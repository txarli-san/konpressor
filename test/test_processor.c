#include "../src/processor.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

int main() {
    AudioProcessor proc;
    init_processor(&proc);
    
    // Configure
    proc.threshold = 0.5f;
    proc.level = 1.0f;
    proc.attack_ms = 5.0f;
    proc.release_ms = 50.0f;
    
    printf("=== Test 1: Below threshold ===\n");
    float phase = M_PI/4.0f;
    for(int i = 0; i < BLOCK_SIZE; i++) {
        float t = (float)i / SAMPLE_RATE;
        proc.in_a[i] = 0.4f * sinf(2.0f * M_PI * 440.0f * t + phase);
    }
    
    process_block(&proc);
    printf("Input Peak: %.3f, Envelope: %.3f, Threshold: %.3f\n", 
           proc.detector.peak, proc.detector.envelope, proc.threshold);
    printf("First out samples: %.3f %.3f %.3f\n", 
           proc.out[0], proc.out[1], proc.out[2]);
    
    printf("\n=== Test 2: Above threshold ===\n");
    for(int i = 0; i < BLOCK_SIZE; i++) {
        float t = (float)i / SAMPLE_RATE;
        proc.in_a[i] = 1.0f * sinf(2.0f * M_PI * 440.0f * t + phase);
    }
    
    process_block(&proc);
    printf("Input Peak: %.3f, Envelope: %.3f, Threshold: %.3f\n", 
           proc.detector.peak, proc.detector.envelope, proc.threshold);
    printf("First out samples: %.3f %.3f %.3f\n", 
           proc.out[0], proc.out[1], proc.out[2]);
    
    return 0;
}