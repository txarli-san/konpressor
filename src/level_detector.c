#include "level_detector.h"
#include <math.h>

void init_level_detector(LevelDetector *det, float attack_ms, float release_ms) {
    det->peak = 0.0f;
    det->rms = 0.0f;
    det->envelope = 0.0f;
    det->attack_coef = 0.1f;  // Fast attack
    det->release_coef = 0.9995f; // Slow release
}

void process_level_detector(LevelDetector *det, float *input, int nframes) {
    float current_peak = 0.0f;
    float sum_squared = 0.0f;
    
    for(int i = 0; i < nframes; i++) {
        float abs_sample = fabsf(input[i]);
        current_peak = fmaxf(current_peak, abs_sample);
        sum_squared += input[i] * input[i];
    }
    
    det->peak = current_peak;
    det->rms = sqrtf(sum_squared / nframes);
    
    if(current_peak > det->envelope) {
        det->envelope = current_peak * (1.0f - det->attack_coef) + 
                       det->envelope * det->attack_coef;
    } else {
        det->envelope = current_peak * (1.0f - det->release_coef) + 
                       det->envelope * det->release_coef;
    }
}