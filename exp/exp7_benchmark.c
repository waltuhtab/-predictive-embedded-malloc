#include <stdio.h>
#include <stdlib.h>
#include "adaptive_mem.h"

#define TRIALS 10000

void run_rigorous_iot_trial(int seed, int* success_count, int* fail_count) {
    srand(seed);
    void* heart_beats[200] = {NULL};
    int beat_idx = 0;

    // Simulate intense, randomized biometric logging
    for (int i = 0; i < 60; i++) {
        // Randomly choose between 8 and 24 bytes to create jagged fragmentation
        int size = (rand() % 17) + 8; 
        if (beat_idx < 200) heart_beats[beat_idx++] = mem_alloc(size);
    }

    // Free random blocks to simulate real-world unpredictable memory churn
    for (int i = 0; i < beat_idx; i++) {
        if ((rand() % 100) < 60 && heart_beats[i] != NULL) { // 60% chance to free
            mem_free(heart_beats[i]);
            heart_beats[i] = NULL;
        }
    }

    // The Ultimate Test: Attempt the massive Cloud Upload buffer
    void* cloud_buffer = mem_alloc(1200);
    if (cloud_buffer != NULL) {
        (*success_count)++;
        mem_free(cloud_buffer);
    } else {
        (*fail_count)++;
    }

    for (int i = 0; i < beat_idx; i++) {
        if (heart_beats[i] != NULL) mem_free(heart_beats[i]);
    }
}

int main() {
    printf("--- EXP 7: 10,000-TRIAL MONTE CARLO BENCHMARK ---\n\n");
    
    AllocStrategy strategies[] = {STRAT_WORST_FIT, STRAT_BEST_FIT, STRAT_PREDICTIVE};
    const char* names[] = {"Traditional (Worst Fit)", "Static (Best Fit)", "Predictive Kernel"};
    
    for (int s = 0; s < 3; s++) {
        int total_success = 0;
        int total_fails = 0;
        
        for (int trial = 0; trial < TRIALS; trial++) {
            mem_init();
            current_strategy = strategies[s];
            
            // We pass 'trial' as the random seed so EVERY strategy faces 
            // the exact same 10,000 random sequences. It is scientifically fair.
            run_rigorous_iot_trial(trial, &total_success, &total_fails);
        }
        
        float win_rate = ((float)total_success / TRIALS) * 100.0f;
        printf("[%s]\n", names[s]);
        printf("  -> Successes: %d / %d (%.2f%%)\n", total_success, TRIALS, win_rate);
        printf("  -> Fails:     %d / %d\n\n", total_fails, TRIALS);
    }
    
    return 0;
}