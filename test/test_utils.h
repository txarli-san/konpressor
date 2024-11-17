#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "../src/config.h"

// Generate a sine wave with RMS normalization
float generate_sine(float freq, int sample);

// Generate a sine wave at specific dB level
float generate_sine_at_db(float freq, int sample, float db_level);

#endif