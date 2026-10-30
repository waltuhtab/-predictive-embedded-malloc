#include <stdio.h>
#include "adaptive_mem.h"

void run_bursty_workload() {
    void* ptrs[100] = {NULL};
    int count = 0;
    
    // Fill up most of the memory with 32-byte blocks
    for (int i = 0; i < 40; i++) ptrs[count++] = mem_alloc(32); 
    
    // Free alternating blocks to create "checkerboard" fragmentation
    for (int i = 0; i < 40; i += 2) {
        mem_free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    // Request smaller blocks to test hole utilization
    for (int i = 0; i < 20; i++) ptrs[count++] = mem_alloc(16);
    
    // The Ultimate Test: request a massive block
    void* huge_chunk = mem_alloc(1200);
    if (huge_chunk != NULL) mem_free(huge_chunk);
    
    // Clean up
    for (int i = 0; i < count; i++) {
        if (ptrs[i] != NULL) mem_free(ptrs[i]);
    }
}

int main() {
    printf("--- EXP 1: ADAPTIVE ALGORITHM STRESS TEST ---\n\n");
    
    mem_init();
    current_strategy = STRAT_FIRST_FIT;
    run_bursty_workload();
    print_experiment_results("First Fit");
    
    mem_init();
    current_strategy = STRAT_BEST_FIT;
    run_bursty_workload();
    print_experiment_results("Best Fit");

    mem_init();
    current_strategy = STRAT_WORST_FIT;
    run_bursty_workload();
    print_experiment_results("Worst Fit");
    
    mem_init();
    current_strategy = STRAT_ADAPTIVE;
    run_bursty_workload();
    print_experiment_results("Adaptive V3");
    
    return 0;
}