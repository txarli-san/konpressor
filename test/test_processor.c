#include "../src/processor.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

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

    // TDD Step 1a: Test corrected signal flow - in_b compressed based on in_a
    printf("\n=== TDD Step 1a: Corrected Signal Flow Test ===\n");
    init_processor(&proc);  // Reset processor
    set_sidechain(&proc, 1);
    set_threshold(&proc, -20.0f);
    set_ratio(&proc, 4.0f);
    set_attack(&proc, 10.0f);
    set_release(&proc, 100.0f);

    // in_a: loud control signal (-6dB, above threshold)
    // in_b: main signal (-24dB)
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine_at_db(440.0f, i, -6.0f);
        proc.in_b[i] = generate_sine_at_db(880.0f, i, -24.0f);
    }

    // Process multiple blocks to settle detector
    for(int i = 0; i < 10; i++) {
        process_block(&proc);
    }

    float in_a_level = compute_rms_level(proc.in_a, BLOCK_SIZE);
    float in_b_level = compute_rms_level(proc.in_b, BLOCK_SIZE);
    float out_level = compute_rms_level(proc.out, BLOCK_SIZE);

    printf("Control signal (in_a) level: %.1f dB\n", in_a_level);
    printf("Main signal (in_b) level: %.1f dB\n", in_b_level);
    printf("Output level: %.1f dB\n", out_level);
    printf("Gain reduction: %.1f dB\n", proc.current_gr);

    // Assertions for corrected flow:
    // 1. Control signal in_a should be above threshold, triggering compression
    TEST_ASSERT(in_a_level > proc.threshold, "Control signal should be above threshold");
    // 2. Gain reduction should be applied (negative value)
    TEST_ASSERT(proc.current_gr < -1.0f, "Gain reduction should be significant");
    // 3. Output level should be in_b level + gain reduction (compressed)
    float expected_out_level = in_b_level + proc.current_gr;
    TEST_ASSERT_FLOAT_EQ(out_level, expected_out_level, 1.0f, "Output should be compressed in_b");
    // 4. Output should not match uncompressed in_b
    TEST_ASSERT(out_level < in_b_level - 1.0f, "Output should be quieter than uncompressed in_b");

    printf("TDD Step 1a test completed\n");

    // TDD Step 1b: Test level parameter as makeup gain
    printf("\n=== TDD Step 1b: Makeup Gain Test ===\n");
    init_processor(&proc);  // Reset
    set_threshold(&proc, -20.0f);
    set_ratio(&proc, 4.0f);
    set_attack(&proc, 10.0f);
    set_release(&proc, 100.0f);

    // Set level to +6dB makeup gain
    set_level(&proc, 2.0f);  // 6dB = 2.0 linear

    // in_a: control signal above threshold
    // in_b: main signal
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine_at_db(440.0f, i, -6.0f);
        proc.in_b[i] = generate_sine_at_db(880.0f, i, -12.0f);
    }

    // Process to settle
    for(int i = 0; i < 10; i++) {
        process_block(&proc);
    }

    float in_b_level_1b = compute_rms_level(proc.in_b, BLOCK_SIZE);
    float out_level_1b = compute_rms_level(proc.out, BLOCK_SIZE);
    float expected_out_no_makeup = in_b_level_1b + proc.current_gr;
    float expected_out_with_makeup = expected_out_no_makeup + 6.0f;  // +6dB makeup

    printf("Main signal (in_b) level: %.1f dB\n", in_b_level_1b);
    printf("Output level: %.1f dB\n", out_level_1b);
    printf("Gain reduction: %.1f dB\n", proc.current_gr);
    printf("Expected output without makeup: %.1f dB\n", expected_out_no_makeup);
    printf("Expected output with +6dB makeup: %.1f dB\n", expected_out_with_makeup);

    // Currently level is not applied, so this will fail
    TEST_ASSERT_FLOAT_EQ(out_level_1b, expected_out_with_makeup, 1.0f, "Output should include makeup gain");

    printf("TDD Step 1b test completed\n");

    // TDD Step 2a: Test mix parameter addition and initialization
    printf("\n=== TDD Step 2a: Mix Parameter Test ===\n");
    init_processor(&proc);  // Reset

    // Test that mix parameter exists and initializes to default (0.0f)
    printf("Mix parameter: %.2f\n", proc.mix);
    TEST_ASSERT_FLOAT_EQ(proc.mix, 0.0f, 0.01f, "Mix should initialize to 0.0");

    printf("TDD Step 2a test completed\n");

    // TDD Step 2b: Test mix logic - blending compressed_b and in_a
    printf("\n=== TDD Step 2b: Mix Logic Test ===\n");

    // Test mix = 0.0 (only compressed in_b)
    init_processor(&proc);
    set_threshold(&proc, -20.0f);
    set_ratio(&proc, 4.0f);
    set_attack(&proc, 10.0f);
    set_release(&proc, 100.0f);
    set_level(&proc, 1.0f);  // No makeup for this test
    proc.mix = 0.0f;  // Direct set since no setter yet

    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc.in_a[i] = generate_sine_at_db(440.0f, i, -6.0f);   // Loud control
        proc.in_b[i] = generate_sine_at_db(880.0f, i, -12.0f);  // Main signal
    }

    for(int i = 0; i < 10; i++) {
        process_block(&proc);
    }

    float compressed_b_level = compute_rms_level(proc.out, BLOCK_SIZE);
    printf("Mix=0.0: Output level %.1f dB (should equal compressed in_b)\n", compressed_b_level);

    // Test mix = 1.0 (only in_a)
    proc.mix = 1.0f;
    for(int i = 0; i < 10; i++) {
        process_block(&proc);
    }

    float in_a_level_2b = compute_rms_level(proc.in_a, BLOCK_SIZE);
    float mix1_out_level = compute_rms_level(proc.out, BLOCK_SIZE);
    printf("Mix=1.0: Output level %.1f dB, in_a level %.1f dB\n", mix1_out_level, in_a_level_2b);
    TEST_ASSERT_FLOAT_EQ(mix1_out_level, in_a_level_2b, 0.1f, "Mix=1.0 should output in_a");

    // Test mix = 0.5 (50/50 blend)
    proc.mix = 0.5f;
    for(int i = 0; i < 10; i++) {
        process_block(&proc);
    }

    float mix05_out_level = compute_rms_level(proc.out, BLOCK_SIZE);
    // For blend, check that it's between the two extremes
    printf("Mix=0.5: Output level %.1f dB (should be between %.1f and %.1f)\n",
           mix05_out_level, compressed_b_level, in_a_level_2b);
    TEST_ASSERT(mix05_out_level > compressed_b_level - 1.0f && mix05_out_level < in_a_level_2b + 1.0f,
                "Mix=0.5 should blend between compressed_b and in_a");

    printf("TDD Step 2b test completed\n");

    return 0;
}