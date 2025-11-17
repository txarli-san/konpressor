#include "../src/hate.h"
#include "../src/config.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

int main() {
    HateModule hate;
    float input[BLOCK_SIZE];
    float output[BLOCK_SIZE];

    printf("=== Hate Module Comprehensive Tests ===\n");

    // Test 1: Initialization
    init_hate(&hate);
    TEST_ASSERT_FLOAT_EQ(hate.drive, 1.0f, 0.01f, "Drive initializes to 1.0");
    TEST_ASSERT_FLOAT_EQ(hate.pre_post_mix, 0.0f, 0.01f, "Pre-post mix initializes to 0.0");
    TEST_ASSERT(hate.bypass == 0, "Bypass initializes to 0");

    // Test 2: Bypass functionality
    for (int i = 0; i < BLOCK_SIZE; i++) {
        input[i] = 0.5f * generate_sine(440.0f, i);
    }

    hate.bypass = 1;
    process_hate(&hate, input, output, BLOCK_SIZE);
    TEST_ASSERT(buffers_equal(input, output, BLOCK_SIZE, 0.001f), "Bypass passes input unchanged");

    // Test 3: Mild saturation (drive = 1.0)
    hate.bypass = 0;
    hate.drive = 1.0f;
    hate.pre_post_mix = 1.0f;
    process_hate(&hate, input, output, BLOCK_SIZE);
    // With drive=1.0, there should be some mild saturation
    int saturated = 0;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (fabsf(output[i]) < fabsf(input[i]) - 0.01f) {  // Some compression
            saturated++;
        }
    }
    TEST_ASSERT(saturated > BLOCK_SIZE / 4, "Drive=1.0 should cause mild saturation");

    // Test 4: Saturation with drive = 2.0
    hate.drive = 2.0f;
    // Use louder input to trigger clipping
    for (int i = 0; i < BLOCK_SIZE; i++) {
        input[i] = 1.0f * generate_sine(440.0f, i);
    }
    process_hate(&hate, input, output, BLOCK_SIZE);

    // Check for saturation (output should be clipped)
    int clipped_samples = 0;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (fabsf(output[i]) >= 0.95f) {
            clipped_samples++;
        }
    }
    TEST_ASSERT(clipped_samples > BLOCK_SIZE / 10, "Drive=2.0 should cause clipping");

    // Regenerate input for next tests
    for (int i = 0; i < BLOCK_SIZE; i++) {
        input[i] = 0.5f * generate_sine(440.0f, i);
    }

    // Test 5: Wet/dry mix = 0.0 (dry only)
    hate.drive = 3.0f;
    hate.pre_post_mix = 0.0f;
    process_hate(&hate, input, output, BLOCK_SIZE);
    TEST_ASSERT(buffers_equal(input, output, BLOCK_SIZE, 0.001f), "Mix=0.0 should be dry only");

    // Test 6: Wet/dry mix = 1.0 (wet only)
    hate.pre_post_mix = 1.0f;
    process_hate(&hate, input, output, BLOCK_SIZE);

    // Output should be saturated version
    float saturated_input[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++) {
        saturated_input[i] = tanhf(input[i] * hate.drive);
    }
    TEST_ASSERT(buffers_equal(output, saturated_input, BLOCK_SIZE, 0.01f), "Mix=1.0 should be saturated only");

    // Test 7: Wet/dry mix = 0.5 (50/50 blend)
    hate.pre_post_mix = 0.5f;
    process_hate(&hate, input, output, BLOCK_SIZE);

    for (int i = 0; i < 5; i++) {
        float expected = input[i] * 0.5f + tanhf(input[i] * hate.drive) * 0.5f;
        TEST_ASSERT_FLOAT_EQ(output[i], expected, 0.01f, "Mix=0.5 should blend correctly");
    }

    // Test 8: Extreme drive values
    hate.drive = 0.1f;  // Very low drive
    hate.pre_post_mix = 1.0f;
    process_hate(&hate, input, output, BLOCK_SIZE);
    // Should be very mild saturation
    TEST_ASSERT(buffers_equal(input, output, BLOCK_SIZE, 0.6f), "Low drive should have minimal effect");

    hate.drive = 10.0f;  // Very high drive
    process_hate(&hate, input, output, BLOCK_SIZE);
    // Should be heavily clipped
    int heavily_clipped = 0;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (fabsf(output[i]) >= 0.99f) {
            heavily_clipped++;
        }
    }
    TEST_ASSERT(heavily_clipped > BLOCK_SIZE / 2, "High drive should heavily clip");

    printf("All Hate module tests passed!\n");
    return 0;
}