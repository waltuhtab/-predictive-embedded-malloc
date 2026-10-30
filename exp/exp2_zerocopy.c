#include <stdio.h>
#include "adaptive_mem.h"

void run_zerocopy_workload() {
    printf("--- EXP 2: ZERO-COPY EFFICIENCY TEST ---\n");
    
    // --- TRADITIONAL COPYING ---
    printf("\n[1] Traditional Pipeline (Copying)\n");
    void* sensor_buffer = mem_alloc(256);
    void* processor_buffer = mem_alloc(256); 
    void* radio_buffer = mem_alloc(256);
    
    printf("RAM Used: %lu bytes | Allocations Performed: %lu\n", 
           stats.memory_used, stats.total_allocations);
           
    mem_free(sensor_buffer);
    mem_free(processor_buffer);
    mem_free(radio_buffer);

    stats.total_allocations = 0;
    stats.memory_used = 0;

    // --- ZERO-COPY ARCHITECTURE ---
    printf("\n[2] Zero-Copy Pipeline (Reference Counting)\n");
    void* zc_sensor_buffer = mem_alloc(256);
    void* zc_processor_buffer = mem_share(zc_sensor_buffer);
    void* zc_radio_buffer = mem_share(zc_processor_buffer);

    printf("RAM Used: %lu bytes | Allocations Performed: %lu\n", 
           stats.memory_used, stats.total_allocations);
           
    mem_free(zc_sensor_buffer); 
    mem_free(zc_processor_buffer);
    mem_free(zc_radio_buffer);
    printf("\n");
}

int main() {
    mem_init();
    run_zerocopy_workload();
    return 0;
}