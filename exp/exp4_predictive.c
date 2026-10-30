#include <stdio.h>
#include "adaptive_mem.h"

void run_stealth_workload() {
    void* ptrs[100] = {NULL};
    int count = 0;
    
    // 1. Stealth Storm: 20 tiny allocations.
    // Adaptive sees low memory usage and low fragmentation, so it uses Worst Fit.
    // Worst Fit slowly chews away the massive block at the end of memory.
    for (int i = 0; i < 20; i++) {
        ptrs[count++] = mem_alloc(16);
    }
    
    // 2. Free alternating blocks (creates tiny holes)
    // Fragmentation registers at ~10% because the massive end-block is still huge.
    for (int i = 0; i < 20; i += 2) {
        mem_free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    // 3. Round Two of the Storm.
    // Adaptive STILL thinks everything is fine (frag < 20%) and chews the end block again.
    // Predictive realizes average size is 16 bytes, predicts a storm, and packs the holes!
    for (int i = 0; i < 20; i++) {
        ptrs[count++] = mem_alloc(16);
    }
    
    // 4. The Boss Fight: A giant 2800-byte allocation.
    // If the algorithm chewed the main block too much (Adaptive V3), this fails.
    void* giant = mem_alloc(2800);
    if (giant != NULL) {
        mem_free(giant);
    }
    
    for (int i = 0; i < count; i++) {
        if (ptrs[i] != NULL) mem_free(ptrs[i]);
    }
}

int main() {
    printf("--- EXP 4: PREDICTIVE vs REACTIVE ADAPTATION ---\n\n");
    
    mem_init();
    current_strategy = STRAT_ADAPTIVE;
    run_stealth_workload();
    print_experiment_results("Adaptive V3");
    
    mem_init();
    current_strategy = STRAT_PREDICTIVE;
    run_stealth_workload();
    print_experiment_results("Predictive");
    
    return 0;
}