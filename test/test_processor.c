// test/test_processor.c
#include "../src/processor.h"
#include "test_utils.h"
#include <stdio.h>

int main() {
    AudioProcessor proc;
    init_processor(&proc);
    
    // Generate test signals
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine(440.0f, i);
        proc.in_b[i] = generate_sine(880.0f, i);
    }
    
    // Test different configurations
    printf("Normal processing:\n");
    process_block(&proc);
    for(int i = 0; i < 5; i++) {
        printf("Sample %d: In=%.3f Filtered=%.3f Out=%.3f\n",
               i, proc.in_a[i], proc.filtered[i], proc.out[i]);
    }
    
    printf("\nWith bypass:\n");
    set_bypass(&proc, 1);
    process_block(&proc);
    for(int i = 0; i < 5; i++) {
        printf("Sample %d: In=%.3f Filtered=%.3f Out=%.3f\n",
               i, proc.in_a[i], proc.filtered[i], proc.out[i]);
    }
    
    return 0;
}