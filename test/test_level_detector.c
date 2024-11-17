#include "../src/level_detector.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

#define TEST_BLOCK_SIZE 32

int main() {
    LevelDetector det;
    float block[TEST_BLOCK_SIZE];
    
    // Test 1: Normal signal
    printf("Test 1: Normal level signal\n");
    init_level_detector(&det, -20.0f, 4.0f, 10.0f, 100.0f, 6.0f);
    
    // Generate test signal
    for(int i = 0; i < TEST_BLOCK_SIZE; i++) {
        block[i] = generate_sine(440.0f, i) * 0.5f; // -6dB signal
    }
    
    float gain = process_level_detector(&det, block, TEST_BLOCK_SIZE);
    printf("Input Level: %.1f dB\n", det.current_level);
    printf("Gain Reduction: %.1f dB\n", det.current_gr);
    printf("Output Gain: %.3f\n\n", gain);
    
    // Test 2: Hot signal
    printf("Test 2: Hot signal\n");
    init_level_detector(&det, -20.0f, 4.0f, 10.0f, 100.0f, 6.0f);
    
    // Generate louder test signal
    for(int i = 0; i < TEST_BLOCK_SIZE; i++) {
        block[i] = generate_sine(440.0f, i) * 2.0f; // +6dB signal
    }
    
    // Process multiple blocks to see envelope behavior
    printf("Block,InputLevel,GainReduction,OutputGain\n");
    for(int block_num = 0; block_num < 10; block_num++) {
        gain = process_level_detector(&det, block, TEST_BLOCK_SIZE);
        printf("%d,%.1f,%.1f,%.3f\n", 
               block_num, 
               det.current_level,
               det.current_gr,
               gain);
    }
    
    // Test 3: Attack/Release behavior
    printf("\nTest 3: Attack/Release behavior\n");
    init_level_detector(&det, -20.0f, 4.0f, 5.0f, 50.0f, 6.0f);
    
    printf("Block,Signal,InputLevel,GainReduction\n");
    // Attack phase - loud signal
    for(int block_num = 0; block_num < 5; block_num++) {
        for(int i = 0; i < TEST_BLOCK_SIZE; i++) {
            block[i] = generate_sine(440.0f, i) * 2.0f;
        }
        gain = process_level_detector(&det, block, TEST_BLOCK_SIZE);
        printf("%d,loud,%.1f,%.1f\n", 
               block_num,
               det.current_level,
               det.current_gr);
    }
    
    // Release phase - quiet signal
    for(int block_num = 5; block_num < 10; block_num++) {
        for(int i = 0; i < TEST_BLOCK_SIZE; i++) {
            block[i] = generate_sine(440.0f, i) * 0.25f;
        }
        gain = process_level_detector(&det, block, TEST_BLOCK_SIZE);
        printf("%d,quiet,%.1f,%.1f\n", 
               block_num,
               det.current_level,
               det.current_gr);
    }
    
    return 0;
}