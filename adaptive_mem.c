/**
 * ADAPTIVE MEMORY MANAGEMENT LIBRARY - CORE
 * -----------------------------------------
 * Implementation of the memory pool, inline ASM zeroing, 
 * baseline algorithms, Adaptive controller, Memory Safety Layer,
 * and the Time-Series Predictive Engine.
 */

#include "adaptive_mem.h"
#include <stdio.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

#define MAGIC_ALIVE 0xDEADBEEF
#define MAGIC_FREED 0xCAFEBABE

typedef struct Block {
    size_t size;
    bool is_free;
    uint8_t ref_count;
    uint32_t magic;
    struct Block* next;
} Block;

#define META_SIZE ALIGN(sizeof(Block))

static uint8_t memory_pool[MEMORY_SIZE];
static Block* free_list = (void*)memory_pool;

MemStats stats = {0};
AllocStrategy current_strategy = STRAT_FIRST_FIT;

// PREDICTIVE ENGINE STATE
#define HISTORY_SIZE 5
static size_t alloc_history[HISTORY_SIZE] = {0};
static int history_idx = 0;

static inline void asm_fast_zero(void* dest, size_t count) {
    __asm__ volatile (
        "rep stosb"
        : "+D" (dest), "+c" (count)
        : "a" (0)
        : "memory"
    );
}

void mem_init(void) {
    free_list->size = MEMORY_SIZE - META_SIZE;
    free_list->is_free = true;
    free_list->magic = MAGIC_FREED;
    free_list->next = NULL;
    
    stats.total_allocations = 0;
    stats.failed_allocations = 0;
    stats.memory_used = 0;
    stats.security_alerts = 0;
    
    // Reset predictive history
    for(int i=0; i<HISTORY_SIZE; i++) alloc_history[i] = 0;
    history_idx = 0;
}

static void split_block(Block* block, size_t size) {
    if (block->size >= size + META_SIZE + ALIGNMENT) {
        Block* new_block = (Block*)((uint8_t*)block + META_SIZE + size);
        new_block->size = block->size - size - META_SIZE;
        new_block->is_free = true;
        new_block->magic = MAGIC_FREED;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

static void coalesce_blocks(void) {
    Block* curr = free_list;
    while (curr != NULL && curr->next != NULL) {
        if (curr->is_free && curr->next->is_free) {
            curr->size += curr->next->size + META_SIZE;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

static Block* find_first_fit(size_t size) {
    Block* curr = free_list;
    while (curr != NULL) {
        if (curr->is_free && curr->size >= size) return curr;
        curr = curr->next;
    }
    return NULL;
}

static Block* find_best_fit(size_t size) {
    Block* curr = free_list;
    Block* best = NULL;
    while (curr != NULL) {
        if (curr->is_free && curr->size >= size) {
            if (best == NULL || curr->size < best->size) best = curr;
        }
        curr = curr->next;
    }
    return best;
}

static Block* find_worst_fit(size_t size) {
    Block* curr = free_list;
    Block* worst = NULL;
    while (curr != NULL) {
        if (curr->is_free && curr->size >= size) {
            if (worst == NULL || curr->size > worst->size) worst = curr;
        }
        curr = curr->next;
    }
    return worst;
}

static Block* find_adaptive_fit(size_t size) {
    size_t total_free = 0, largest_free = 0;
    Block* curr = free_list;
    
    while (curr != NULL) {
        if (curr->is_free) {
            total_free += curr->size;
            if (curr->size > largest_free) largest_free = curr->size;
        }
        curr = curr->next;
    }
    
    float usage_ratio = (float)stats.memory_used / (float)MEMORY_SIZE;
    float fragmentation = (total_free > 0) ? (1.0f - ((float)largest_free / (float)total_free)) : 0.0f;
    
    if (usage_ratio > 0.70f || fragmentation > 0.20f) {
        return find_best_fit(size);
    } else if (size >= (largest_free / 2)) {
        return find_first_fit(size);
    } else {
        return find_worst_fit(size);
    }
}

static Block* find_predictive_fit(size_t size) {
    // 1. Calculate moving average of recent allocations
    size_t sum = 0;
    int count = 0;
    for (int i = 0; i < HISTORY_SIZE; i++) {
        if (alloc_history[i] > 0) {
            sum += alloc_history[i];
            count++;
        }
    }
    size_t avg_size = (count > 0) ? (sum / count) : 0;

    // 2. PREDICTIVE LOGIC:
    // If recent requests are consistently small (<= 64 bytes), a "Fragmentation Storm" is occurring.
    // Lock into defensive Best Fit immediately, regardless of what the current fragmentation % is.
    if (avg_size > 0 && avg_size <= 64) {
        return find_best_fit(size);
    }

    // 3. Fall back to standard Reactive logic if no storm is detected
    return find_adaptive_fit(size);
}

void* mem_alloc(size_t size) {
    if (size == 0) return NULL;
    stats.total_allocations++;
    size_t aligned_size = ALIGN(size);
    
    // Record allocation size in the sliding predictive window
    alloc_history[history_idx] = aligned_size;
    history_idx = (history_idx + 1) % HISTORY_SIZE;
    
    Block* block = NULL;
    switch(current_strategy) {
        case STRAT_FIRST_FIT:  block = find_first_fit(aligned_size); break;
        case STRAT_BEST_FIT:   block = find_best_fit(aligned_size); break;
        case STRAT_WORST_FIT:  block = find_worst_fit(aligned_size); break;
        case STRAT_ADAPTIVE:   block = find_adaptive_fit(aligned_size); break;
        case STRAT_PREDICTIVE: block = find_predictive_fit(aligned_size); break;
    }
    
    if (block == NULL) {
        stats.failed_allocations++;
        return NULL;
    }
    
    split_block(block, aligned_size);
    
    block->is_free = false;
    block->ref_count = 1;
    block->magic = MAGIC_ALIVE; 
    stats.memory_used += block->size;
    
    void* user_ptr = (void*)((uint8_t*)block + META_SIZE);
    asm_fast_zero(user_ptr, block->size);
    return user_ptr;
}

void* mem_share(void* ptr) {
    if (ptr == NULL) return NULL;
    Block* block = (Block*)((uint8_t*)ptr - META_SIZE);
    if (block->is_free) return NULL;
    block->ref_count++;
    return ptr; 
}

void mem_free(void* ptr) {
    if (ptr == NULL) return;
    Block* block = (Block*)((uint8_t*)ptr - META_SIZE);
    
    if (block->magic != MAGIC_ALIVE && block->magic != MAGIC_FREED) {
        printf("\n[!] SECURITY ALERT: Metadata corruption (buffer overflow) detected near %p!\n", ptr);
        stats.security_alerts++;
        return;
    }
    
    if (block->magic == MAGIC_FREED || block->is_free) {
        printf("\n[!] SECURITY ALERT: Double-free detected at %p!\n", ptr);
        stats.security_alerts++;
        return;
    }
    
    if (block->ref_count > 1) {
        block->ref_count--;
        return; 
    }
    
    block->ref_count = 0;
    block->magic = MAGIC_FREED;
    block->is_free = true;
    stats.memory_used -= block->size;
    coalesce_blocks();
}

void print_experiment_results(const char* strategy_name) {
    printf("Strategy: %-15s | Fails: %2lu | Memory Used (End): %4lu bytes\n", 
           strategy_name, stats.failed_allocations, stats.memory_used);
}