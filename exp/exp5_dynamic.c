#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adaptive_mem.h"

// Helper function to convert strategy enum to string for the CSV
const char* get_strategy_name(AllocStrategy strat) {
    switch(strat) {
        case STRAT_FIRST_FIT: return "First_Fit";
        case STRAT_BEST_FIT: return "Best_Fit";
        case STRAT_WORST_FIT: return "Worst_Fit";
        case STRAT_ADAPTIVE: return "Adaptive_V3";
        case STRAT_PREDICTIVE: return "Predictive";
        default: return "Unknown";
    }
}

// Appends the current stats to a CSV file
void export_to_csv(const char* filename, AllocStrategy strat, int target_allocs, int max_size) {
    FILE* f = fopen(filename, "a");
    if (!f) {
        printf("[!] Error: Could not open %s for writing.\n", filename);
        return;
    }
    
    // Check if file is empty to write the CSV header row
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0) {
        fprintf(f, "Strategy,Target_Allocs,Max_Size,Total_Allocs,Fails,Memory_Used,Security_Alerts\n");
    }
    
    // Write the data row
    fprintf(f, "%s,%d,%d,%lu,%lu,%lu,%lu\n",
            get_strategy_name(strat),
            target_allocs,
            max_size,
            stats.total_allocations,
            stats.failed_allocations,
            stats.memory_used,
            stats.security_alerts);
            
    fclose(f);
}

void run_randomized_workload(int target_allocs, int max_size) {
    void* ptrs[5000] = {NULL}; // Array to track our allocations
    int active_count = 0;
    
    // We use a fixed seed so every strategy faces the EXACT same "random" sequence
    srand(42); 
    
    for (int i = 0; i < target_allocs; i++) {
        // 1. Generate a random allocation size between 8 and max_size
        int request_size = (rand() % (max_size - 8 + 1)) + 8;
        
        // 2. Perform the allocation
        if (active_count < 5000) {
            ptrs[active_count] = mem_alloc(request_size);
            if (ptrs[active_count] != NULL) {
                active_count++;
            }
        }
        
        // 3. Randomly free an older block 30% of the time to simulate realistic memory churn
        if (active_count > 0 && (rand() % 100) < 30) {
            int random_index = rand() % active_count;
            if (ptrs[random_index] != NULL) {
                mem_free(ptrs[random_index]);
                ptrs[random_index] = NULL;
            }
        }
    }
    
    // Clean up remaining blocks
    for (int i = 0; i < active_count; i++) {
        if (ptrs[i] != NULL) mem_free(ptrs[i]);
    }
}

int main(int argc, char* argv[]) {
    // Check if the user provided the correct number of arguments
    if (argc != 5) {
        printf("Usage: run_exp5 <strategy_id> <num_allocations> <max_size> <output.csv>\n");
        printf("Strategies: 0=First, 1=Best, 2=Worst, 3=Adaptive, 4=Predictive\n");
        return 1;
    }
    
    // Parse user inputs
    AllocStrategy strat = atoi(argv[1]);
    int target_allocs = atoi(argv[2]);
    int max_size = atoi(argv[3]);
    const char* csv_file = argv[4];
    
    // Initialize library
    mem_init();
    current_strategy = strat;
    
    // Run simulation and export
    run_randomized_workload(target_allocs, max_size);
    export_to_csv(csv_file, strat, target_allocs, max_size);
    
    printf("-> Finished simulating %s. Data appended to %s\n", get_strategy_name(strat), csv_file);
    return 0;
}