#include "../src/level_detector.h"
#include "test_utils.h"
#include <stdio.h>

int main() {
    LevelDetector det;
    float test_signal[BLOCK_SIZE];
    
    // Test 1: Constant amplitude
    printf("Test 1: Constant amplitude sine\n");
    init_level_detector(&det, 10.0f, 100.0f);
    
    for(int block = 0; block < 10; block++) {
        for(int i = 0; i < BLOCK_SIZE; i++) {
            test_signal[i] = 0.5f * generate_sine(440.0f, block * BLOCK_SIZE + i);
        }
        
        process_level_detector(&det, test_signal, BLOCK_SIZE);
        printf("Block %d: Peak=%.3f RMS=%.3f Env=%.3f\n", 
               block, det.peak, det.rms, det.envelope);
    }
    
    // Test 2: Amplitude jump
    printf("\nTest 2: Amplitude jump\n");
    init_level_detector(&det, 10.0f, 100.0f);
    
    for(int block = 0; block < 10; block++) {
        float amp = (block >= 5) ? 1.0f : 0.1f;
        for(int i = 0; i < BLOCK_SIZE; i++) {
            test_signal[i] = amp * generate_sine(440.0f, block * BLOCK_SIZE + i);
        }
        
        process_level_detector(&det, test_signal, BLOCK_SIZE);
        printf("Block %d: Peak=%.3f RMS=%.3f Env=%.3f\n", 
               block, det.peak, det.rms, det.envelope);
    }
    
    return 0;
}