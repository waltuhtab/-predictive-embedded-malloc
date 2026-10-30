#include <stdio.h>
#include "adaptive_mem.h"

void run_iot_medical_sensor() {
    void* heart_beats[200] = {NULL};
    int beat_idx = 0;
    int successful_syncs = 0;
    int failed_syncs = 0;

    // Simulate 4 "Cloud Sync" operational cycles
    for (int cycle = 1; cycle <= 4; cycle++) {
        
        // 1. Collect constant stream of tiny biometric data
        for (int i = 0; i < 40; i++) {
            if (beat_idx < 200) {
                // Allocate 16-byte struct: { uint32_t timestamp; int bpm; }
                heart_beats[beat_idx++] = mem_alloc(16); 
            }
        }

        // 2. Clear out the oldest readings (rolling window), creating severe fragmentation
        for (int i = 0; i < beat_idx; i += 2) {
            if (heart_beats[i] != NULL) {
                mem_free(heart_beats[i]);
                heart_beats[i] = NULL;
            }
        }

        // 3. Attempt to allocate a massive buffer to compress and upload data via Wi-Fi
        void* cloud_buffer = mem_alloc(1500);
        if (cloud_buffer != NULL) {
            successful_syncs++;
            mem_free(cloud_buffer); // Free after successful upload
        } else {
            failed_syncs++;
        }
    }

    // Clean up remaining memory at end of simulation
    for (int i = 0; i < beat_idx; i++) {
        if (heart_beats[i] != NULL) mem_free(heart_beats[i]);
    }

    printf("  -> Cloud Uploads Succeeded: %d/4\n", successful_syncs);
    printf("  -> Critical Data Lost:      %d/4\n", failed_syncs);
    printf("  -> Core Allocator Fails:    %lu\n", stats.failed_allocations);
}

int main() {
    printf("--- EXP 6: REAL-WORLD IOT MEDICAL SENSOR ---\n\n");
    
    printf("[System A] Traditional RTOS (Worst Fit)\n");
    mem_init();
    current_strategy = STRAT_WORST_FIT;
    run_iot_medical_sensor();
    
    printf("\n[System B] Next-Gen Kernel (Predictive)\n");
    mem_init();
    current_strategy = STRAT_PREDICTIVE;
    run_iot_medical_sensor();
    
    return 0;
}