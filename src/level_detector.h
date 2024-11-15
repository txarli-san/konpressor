#ifndef LEVEL_DETECTOR_H
#define LEVEL_DETECTOR_H

#include "config.h"

typedef struct {
    float peak;
    float rms;
    float attack_coef;
    float release_coef;
    float envelope;
} LevelDetector;

void init_level_detector(LevelDetector *det, float attack_ms, float release_ms);
void process_level_detector(LevelDetector *det, float *input, int nframes);

#endif