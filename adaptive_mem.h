/**
 * ADAPTIVE MEMORY MANAGEMENT LIBRARY - HEADER
 * -------------------------------------------
 * Defines the public API, data structures, and configuration
 * for the adaptive embedded memory allocator.
 */

#ifndef ADAPTIVE_MEM_H
#define ADAPTIVE_MEM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Simulated 4KB RAM pool
#define MEMORY_SIZE 4096 

// Available allocation strategies
typedef enum {
    STRAT_FIRST_FIT,
    STRAT_BEST_FIT,
    STRAT_WORST_FIT,
    STRAT_ADAPTIVE,
    STRAT_PREDICTIVE
} AllocStrategy;

// Statistics for experimental measurement
typedef struct {
    size_t total_allocations;
    size_t failed_allocations;
    size_t memory_used;
    size_t max_fragmentation_events;
    size_t security_alerts; // Tracks memory corruption/double frees
} MemStats;

// Expose the global state so experiment files can read/write them
extern MemStats stats;
extern AllocStrategy current_strategy;

// --- PUBLIC API ---

// Initializes the memory pool. Call this before starting an experiment.
void mem_init(void);

// Allocates 'size' bytes based on the 'current_strategy'.
void* mem_alloc(size_t size);

// Grants shared ownership of an existing memory block (Zero-Copy).
void* mem_share(void* ptr);

// Decrements reference count, or frees memory if no longer shared.
void mem_free(void* ptr);

// Helper to print standard experiment results
void print_experiment_results(const char* strategy_name);

#endif // ADAPTIVE_MEM_H