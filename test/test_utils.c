#include "test_utils.h"
#include <math.h>

#define RMS_NORMALIZE 1.4142135623730951f  // sqrt(2)
#define LEVEL_ADJUST 0.9330329915368074f   // 10^(-0.66/20) to compensate for the error

float generate_sine(float freq, int sample) {
    return sinf(2.0f * M_PI * freq * sample / SAMPLE_RATE) * RMS_NORMALIZE * LEVEL_ADJUST;
}

float generate_sine_at_db(float freq, int sample, float db_level) {
    float amplitude = powf(10.0f, db_level/20.0f);
    return generate_sine(freq, sample) * amplitude;
}