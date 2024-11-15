#include "test_utils.h"
#include <math.h>

float generate_sine(float freq, int sample) {
    return sinf(2.0f * M_PI * freq * sample / SAMPLE_RATE);
}