#include "../src/bypass.h"
#include "test_utils.h"
#include <stdio.h>

int main() {
    BypassModule bypass;
    init_bypass(&bypass);
    
    // Fill test signals
    for(int i = 0; i < BLOCK_SIZE; i++) {
        bypass.input_a[i] = generate_sine(440.0f, i);
        bypass.input_b[i] = generate_sine(880.0f, i);
    }
    
    // Test bypass enabled
    bypass.bypass_enabled = 1;
    process_bypass(&bypass);
    printf("Bypass enabled test:\n");
    for(int i = 0; i < 5; i++) {
        printf("Sample %d: A=%.3f B=%.3f Out=%.3f\n",
               i, bypass.input_a[i], bypass.input_b[i], bypass.output[i]);
    }
    
    // Test bypass disabled
    bypass.bypass_enabled = 0;
    process_bypass(&bypass);
    printf("\nBypass disabled test:\n");
    for(int i = 0; i < 5; i++) {
        printf("Sample %d: A=%.3f B=%.3f Out=%.3f\n",
               i, bypass.input_a[i], bypass.input_b[i], bypass.output[i]);
    }
    
    return 0;
}