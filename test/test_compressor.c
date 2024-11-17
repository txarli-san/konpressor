#include "../src/processor.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

static float compute_rms_level(const float *buffer, int size) {
    float sum_squared = 0.0f;
    for(int i = 0; i < size; i++) {
        sum_squared += buffer[i] * buffer[i];
    }
    return 20.0f * log10f(sqrtf(sum_squared / size));
}

int main() {
    AudioProcessor proc;
    init_processor(&proc);
    
    // Test 1: Bypass with -12dB signal
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine_at_db(440.0f, i, -12.0f);
    }
    
    set_bypass(&proc, 1);
    process_block(&proc);
    
    printf("--- Bypass Test (should be unity gain) ---\n");
    float in_level = compute_rms_level(proc.in_a, BLOCK_SIZE);
    float out_level = compute_rms_level(proc.out, BLOCK_SIZE);
    printf("Input Level: %.1f dB\n", in_level);
    printf("Output Level: %.1f dB\n", out_level);
    printf("Gain Change: %.1f dB\n\n", out_level - in_level);
    
    // Test 2: Compression (-6dB signal, -20dB threshold)
    printf("--- Compression Test ---\n");
    set_bypass(&proc, 0);
    set_threshold(&proc, -20.0f);
    set_ratio(&proc, 4.0f);
    set_attack(&proc, 10.0f);
    set_release(&proc, 100.0f);
    
    // Generate -6dB signal (should be over threshold)
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine_at_db(440.0f, i, -6.0f);
    }
    
    // Process multiple blocks to let detector settle
    for(int i = 0; i < 10; i++) {
        process_block(&proc);
    }
    
    in_level = compute_rms_level(proc.in_a, BLOCK_SIZE);
    out_level = compute_rms_level(proc.out, BLOCK_SIZE);
    float over_threshold = in_level - proc.threshold;
    
    printf("Input Level: %.1f dB\n", in_level);
    printf("Output Level: %.1f dB\n", out_level);
    printf("Amount Over Threshold: %.1f dB\n", over_threshold);
    printf("Gain Reduction: %.1f dB\n", out_level - in_level);
    
    if (over_threshold > 0) {
        float out_over_threshold = out_level - proc.threshold;
        float measured_ratio = over_threshold / out_over_threshold;
        printf("Measured Ratio: %.1f:1\n\n", measured_ratio);
    }
    
    // Test 3: Below Threshold (-24dB signal)
    printf("--- Below Threshold Test ---\n");
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine_at_db(440.0f, i, -24.0f);
    }
    
    process_block(&proc);
    
    in_level = compute_rms_level(proc.in_a, BLOCK_SIZE);
    out_level = compute_rms_level(proc.out, BLOCK_SIZE);
    printf("Input Level: %.1f dB\n", in_level);
    printf("Output Level: %.1f dB\n", out_level);
    printf("Gain Change: %.1f dB\n", out_level - in_level);
    
    return 0;
}