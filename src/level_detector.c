#include "level_detector.h"
#include <math.h>

void init_level_detector(LevelDetector *det, float attack_ms, float release_ms) {
    det->peak = 0.0f;
    det->rms = 0.0f;
    det->envelope = 0.0f;
    
    // Time constants based on sample blocks
    float attack_samples = (attack_ms * 0.001f * SAMPLE_RATE) / BLOCK_SIZE;
    float release_samples = (release_ms * 0.001f * SAMPLE_RATE) / BLOCK_SIZE;
    
    det->attack_coef = expf(-1.0f / attack_samples);
    det->release_coef = expf(-1.0f / release_samples);
}

void process_level_detector(LevelDetector *det, float *input, int nframes) {
    float sum_squared = 0.0f;
    float current_peak = 0.0f;
    
    for(int i = 0; i < nframes; i++) {
        float abs_sample = fabsf(input[i]);
        if(abs_sample > current_peak) current_peak = abs_sample;
        sum_squared += input[i] * input[i];
    }
    
    det->peak = current_peak;
    det->rms = sqrtf(sum_squared / nframes);
    
    float target = det->peak;
    float coef = (target > det->envelope) ? det->attack_coef : det->release_coef;
    det->envelope = target + coef * (det->envelope - target);
}