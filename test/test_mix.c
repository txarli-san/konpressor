#include "../src/processor.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

int main() {
    AudioProcessor proc;
    float test_signal_a[BLOCK_SIZE];
    float test_signal_b[BLOCK_SIZE];

    printf("=== Mix Blending Comprehensive Tests ===\n");

    // Generate test signals
    for (int i = 0; i < BLOCK_SIZE; i++) {
        test_signal_a[i] = 0.8f * generate_sine(440.0f, i);  // -1.9 dB
        test_signal_b[i] = 0.4f * generate_sine(880.0f, i);  // -8.0 dB
    }

    // Test 1: Mix parameter initialization
    init_processor(&proc);
    TEST_ASSERT_FLOAT_EQ(proc.mix, 0.0f, 0.01f, "Mix initializes to 0.0");

    // Test 2: Mix = 0.0 (only compressed_b)
    proc.mix = 0.0f;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = test_signal_a[i];
        proc.in_b[i] = test_signal_b[i];
    }

    // Disable compression for pure mix test
    set_threshold(&proc, -100.0f);  // Very low threshold, no compression
    set_ratio(&proc, 1.0f);
    proc.hate.bypass = 1;  // Bypass saturation
    set_frequency(&proc, 10.0f);  // High-pass filter that passes test frequencies

    process_block(&proc);
    process_block(&proc);  // Run again to settle filter

    // With no compression, mix=0 should output in_b
    float rms_out = compute_rms_level(proc.out, BLOCK_SIZE);
    float rms_in = compute_rms_level(test_signal_b, BLOCK_SIZE);
    TEST_ASSERT_FLOAT_EQ(rms_out, rms_in, 1.0f, "Mix=0.0 should output in_b rms");

    // Test 3: Mix = 1.0 (only in_a)
    proc.mix = 1.0f;
    process_block(&proc);
    process_block(&proc);  // Run again to settle filter
    rms_out = compute_rms_level(proc.out, BLOCK_SIZE);
    rms_in = compute_rms_level(test_signal_a, BLOCK_SIZE);
    TEST_ASSERT_FLOAT_EQ(rms_out, rms_in, 1.0f, "Mix=1.0 should output in_a rms");

    // Test 4: Mix = 0.5 (equal blend)
    proc.mix = 0.5f;
    process_block(&proc);

    for (int i = 0; i < 5; i++) {
        float expected = test_signal_b[i] * 0.5f + test_signal_a[i] * 0.5f;
        TEST_ASSERT_FLOAT_EQ(proc.out[i], expected, 0.01f, "Mix=0.5 should blend equally");
    }

    // Test 5: Mix with compression
    set_threshold(&proc, -20.0f);
    set_ratio(&proc, 4.0f);
    proc.mix = 0.0f;

    // Make in_a loud to trigger compression
    for (int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = 2.0f * generate_sine(440.0f, i);  // Loud control
        proc.in_b[i] = 0.1f * generate_sine(880.0f, i);  // Quiet main
    }

    process_block(&proc);

    float in_b_level = compute_rms_level(proc.in_b, BLOCK_SIZE);
    float out_level = compute_rms_level(proc.out, BLOCK_SIZE);
    TEST_ASSERT(out_level < in_b_level - 5.0f, "Compression should reduce output level");

    // Test 6: Mix with compressed signal
    proc.mix = 0.3f;  // 30% in_a, 70% compressed_b
    process_block(&proc);

    // Output should be blend of compressed_b and in_a
    // Since compressed_b is quieter, output should be between them
    TEST_ASSERT(out_level > in_b_level - 10.0f, "Mix should include some uncompressed level");

    // Test 7: Edge cases
    proc.mix = 0.0f;
    // Silence inputs
    for (int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = 0.0f;
        proc.in_b[i] = 0.0f;
    }
    process_block(&proc);
    TEST_ASSERT(buffer_contains(proc.out, BLOCK_SIZE, 0.0f, 0.001f), "Silence in should give silence out");

    // Test 8: Full scale signals
    proc.mix = 0.5f;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = 0.9f * generate_sine(440.0f, i);
        proc.in_b[i] = 0.9f * generate_sine(880.0f, i);
    }
    process_block(&proc);

    // Check no clipping (output should be <= 1.0)
    int clipped = 0;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (fabsf(proc.out[i]) > 1.0f) {
            clipped++;
        }
    }
    TEST_ASSERT(clipped == 0, "Mix should not cause clipping with normal inputs");

    printf("All Mix blending tests passed!\n");
    return 0;
}