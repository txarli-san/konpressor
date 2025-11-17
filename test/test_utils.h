#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "../src/config.h"
#include <stdio.h>
#include <math.h>

// Generate a sine wave with RMS normalization
float generate_sine(float freq, int sample);

// Generate a sine wave at specific dB level
float generate_sine_at_db(float freq, int sample, float db_level);

// Simple test assertion macro
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s\n", message); \
            return 1; \
        } else { \
            printf("PASS: %s\n", message); \
        } \
    } while (0)

// Assert two floats are equal within tolerance
#define TEST_ASSERT_FLOAT_EQ(a, b, tol, message) \
    TEST_ASSERT(fabsf((a) - (b)) < (tol), message)

// Assert RMS level is within dB range
#define TEST_ASSERT_RMS_DB(buffer, size, expected_db, tol_db, message) \
    do { \
        float rms = compute_rms_level(buffer, size); \
        TEST_ASSERT_FLOAT_EQ(rms, expected_db, tol_db, message); \
    } while (0)

// Compute RMS level in dB
float compute_rms_level(const float *buffer, int size);

// Check if two buffers are equal within tolerance
int buffers_equal(const float *a, const float *b, int size, float tolerance);

// Check if buffer contains expected values within tolerance
int buffer_contains(const float *buffer, int size, float expected_value, float tolerance);

#endif