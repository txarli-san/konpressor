#ifndef LEVEL_DETECTOR_H
#define LEVEL_DETECTOR_H

#include "config.h"

typedef struct {
    float threshold;      // dB threshold
    float ratio;         // Compression ratio
    float attack_ms;     // Attack time in ms
    float release_ms;    // Release time in ms
    float knee_width;    // Soft knee width in dB
    
    float envelope;      // Current envelope value
    float gain_smooth;   // Smoothed gain reduction
    float attack_coef;   // Attack coefficient
    float release_coef;  // Release coefficient
    
    float current_gr;    // Current gain reduction in dB
    float max_gr;        // Maximum gain reduction seen
    float current_level; // Current input level in dB
} LevelDetector;

void init_level_detector(LevelDetector *det, float threshold, float ratio, 
                        float attack_ms, float release_ms, float knee_width);
float process_level_detector(LevelDetector *det, const float *input, int nframes);

// Reinitialize detector with new parameters - keeps state
void update_detector(LevelDetector *det, float threshold, float ratio,
                    float attack_ms, float release_ms, float knee_width);

#endif