#include "../src/processor.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

static float compute_rms_level(const float *buffer, int size) {
    float sum = 0.0f;
    for(int i = 0; i < size; i++) {
        sum += buffer[i] * buffer[i];
    }
    return 20.0f * log10f(sqrtf(sum / size) + 1e-30f);
}

int main() {
    AudioProcessor proc;
    init_processor(&proc);
    
    // Test 1: No compression
    printf("Test 1: Normal processing (high threshold)\n");
    set_threshold(&proc, 0.0f);      // 0 dB threshold
    set_ratio(&proc, 4.0f);          // 4:1 ratio
    set_attack(&proc, 10.0f);
    set_release(&proc, 100.0f);
    
    // Generate -12dB sine
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine(440.0f, i) * 0.25f;
    }
    
    process_block(&proc);
    
    printf("Sample,Input,Filtered,Output,Level,GR\n");
    for(int i = 0; i < 5; i++) {
        printf("%d,%.3f,%.3f,%.3f,%.1f,%.1f\n",
               i, proc.in_a[i], proc.filtered[i], proc.out[i],
               proc.detector.current_level, proc.current_gr);
    }
    printf("\n--- Signal Levels ---\n");
    printf("Input RMS: %.1f dB\n", compute_rms_level(proc.in_a, BLOCK_SIZE));
    printf("Output RMS: %.1f dB\n", compute_rms_level(proc.out, BLOCK_SIZE));
    printf("Gain Reduction: %.1f dB\n", proc.current_gr);
    
    // Test 2: Compression
    printf("\nTest 2: With compression (low threshold)\n");
    set_threshold(&proc, -20.0f);
    
    // Generate -6dB sine
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine(440.0f, i) * 0.5f;
    }
    
    printf("\nBlock,InputLevel,GainReduction,OutputLevel\n");
    for(int block = 0; block < 5; block++) {
        process_block(&proc);
        float out_level = compute_rms_level(proc.out, BLOCK_SIZE);
        printf("%d,%.1f,%.1f,%.1f\n",
               block, proc.detector.current_level,
               proc.current_gr, out_level);
    }
    
    // Test 3: Sidechain
    printf("\nTest 3: Sidechain processing\n");
    set_sidechain(&proc, 1);
    set_threshold(&proc, -20.0f);
    set_ratio(&proc, 4.0f);
    
    // Main signal quiet (-12dB), sidechain loud (-6dB)
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine(440.0f, i) * 0.25f;  // -12dB main
        proc.in_b[i] = generate_sine(880.0f, i) * 0.5f;   // -6dB sidechain
    }
    
    // Let detector settle
    for(int i = 0; i < 10; i++) {
        process_block(&proc);
    }
    
    printf("Sample,MainIn,SCIn,Output,GR\n");
    for(int i = 0; i < 5; i++) {
        printf("%d,%.3f,%.3f,%.3f,%.1f\n",
               i, proc.in_a[i], proc.in_b[i], 
               proc.out[i], proc.current_gr);
    }
    
    printf("\n--- Sidechain Levels ---\n");
    printf("Main Input RMS: %.1f dB\n", compute_rms_level(proc.in_a, BLOCK_SIZE));
    printf("SC Input RMS: %.1f dB\n", compute_rms_level(proc.in_b, BLOCK_SIZE));
    printf("Output RMS: %.1f dB\n", compute_rms_level(proc.out, BLOCK_SIZE));
    printf("Detector Level: %.1f dB\n", proc.detector.current_level);
    printf("Gain Reduction: %.1f dB\n", proc.current_gr);
    
    // Test 4: Bypass
    printf("\nTest 4: Bypass check\n");
    set_bypass(&proc, 1);
    process_block(&proc);
    
    printf("\nSample,Input,Output,Ratio\n");
    for(int i = 0; i < 5; i++) {
        float ratio = (fabsf(proc.in_a[i]) > 1e-10) ? 
                     fabsf(proc.out[i] / proc.in_a[i]) : 1.0f;
        printf("%d,%.3f,%.3f,%.3f\n", i, proc.in_a[i], proc.out[i], ratio);
    }
    printf("\n--- Bypass Levels ---\n");
    printf("Input RMS: %.1f dB\n", compute_rms_level(proc.in_a, BLOCK_SIZE));
    printf("Output RMS: %.1f dB\n", compute_rms_level(proc.out, BLOCK_SIZE));
    
    return 0;
}