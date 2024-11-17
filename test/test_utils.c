#include "test_utils.h"
#include <math.h>
#include <stdio.h>

#define RMS_NORMALIZE 1.4142135623730951f  // sqrt(2)
#define LEVEL_ADJUST 0.9330329915368074f   // 10^(-0.66/20)

float generate_sine(float freq, int sample) {
    return sinf(2.0f * M_PI * freq * sample / SAMPLE_RATE);
}

float generate_sine_at_db(float freq, int sample, float db_level) {
    float amplitude = powf(10.0f, db_level/20.0f);
    float raw_sine = generate_sine(freq, sample);
    float normalized = raw_sine * RMS_NORMALIZE * LEVEL_ADJUST;
    float result = normalized * amplitude;
    
    // Debug first sample
    if (sample == 0) {
        printf("DEBUG Signal Generation:\n");
        printf("Requested level: %.1f dB\n", db_level);
        printf("Amplitude: %.6f\n", amplitude);
        printf("Raw sine: %.6f\n", raw_sine);
        printf("Normalized: %.6f\n", normalized);
        printf("Final: %.6f\n", result);
    }
    
    return result;
}