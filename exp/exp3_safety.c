#include <stdio.h>
#include <string.h>
#include "adaptive_mem.h"

int main() {
    printf("--- EXP 3: MEMORY SAFETY & CORRUPTION DETECTION ---\n\n");
    mem_init();
    
    // 1. Simulate a Double-Free bug
    printf("[Test A] Attempting Double-Free...\n");
    void* ptr1 = mem_alloc(32);
    mem_free(ptr1); // First free (Legal)
    mem_free(ptr1); // Second free (Bug!)
    
    // 2. Simulate a Buffer Overflow bug
    printf("\n[Test B] Attempting Buffer Overflow...\n");
    void* ptr2 = mem_alloc(16);
    void* ptr3 = mem_alloc(16);
    
    // The developer accidentally writes 32 bytes into a 16-byte buffer!
    // This overwrites ptr3's metadata header.
    memset(ptr2, 0xFF, 32); 
    
    // The system attempts to clean up...
    mem_free(ptr3); 
    
    printf("\nTotal Security Alerts Caught: %lu\n\n", stats.security_alerts);
    return 0;
}