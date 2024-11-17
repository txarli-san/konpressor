#include "level_detector.h"
#include <math.h>
#include <stdio.h>


#define DB_TO_LINEAR(x) powf(10.0f, (x) * 0.05f)
#define LINEAR_TO_DB(x) (20.0f * log10f((x) + 1e-30f))

static float calc_coefficient(float time_ms) {
    return 1.0f - expf(-2.2f / (time_ms * SAMPLE_RATE * 0.001f));
}

void init_level_detector(LevelDetector *det, float threshold, float ratio, 
                        float attack_ms, float release_ms, float knee_width) {
    det->threshold = threshold;
    det->ratio = ratio;
    det->attack_ms = attack_ms;
    det->release_ms = release_ms;
    det->knee_width = knee_width;
    
    det->envelope = 0.0f;
    det->current_gr = 0.0f;
    det->max_gr = 0.0f;
    det->current_level = -120.0f;
    
    det->attack_coef = calc_coefficient(attack_ms);
    det->release_coef = calc_coefficient(release_ms);
}

void update_detector(LevelDetector *det, float threshold, float ratio,
                    float attack_ms, float release_ms, float knee_width) {
    det->threshold = threshold;
    det->ratio = ratio;
    det->attack_ms = attack_ms;
    det->release_ms = release_ms;
    det->knee_width = knee_width;
    
    det->attack_coef = calc_coefficient(attack_ms);
    det->release_coef = calc_coefficient(release_ms);
}

float process_level_detector(LevelDetector *det, const float *input, int nframes) {
    // Calculate current RMS level
    float sum_squared = 0.0f;
    for (int i = 0; i < nframes; i++) {
        sum_squared += input[i] * input[i];
    }
    float current_rms = sqrtf(sum_squared / nframes);
    
    // Set initial envelope if not set
    if (det->envelope <= 0.0f) {
        det->envelope = current_rms;
    }
    
    // Update envelope with correct coefficient application
    if (current_rms > det->envelope) {
        det->envelope = det->envelope + (current_rms - det->envelope) * (1.0f - det->attack_coef);
    } else {
        det->envelope = det->envelope + (current_rms - det->envelope) * (1.0f - det->release_coef);
    }
    
    // Convert to dB for detection
    det->current_level = LINEAR_TO_DB(det->envelope);
    
    printf("RMS: %.1f dB, Envelope: %.1f dB, Threshold: %.1f dB\n",
           LINEAR_TO_DB(current_rms), det->current_level, det->threshold);
    
    // Only apply gain reduction if envelope exceeds threshold
    if (det->current_level <= det->threshold) {
        det->current_gr = 0.0f;
        return 1.0f;  // Unity gain
    }
    
    // Calculate gain reduction in dB
    float over_threshold = det->current_level - det->threshold;
    det->current_gr = -(over_threshold * (1.0f - (1.0f / det->ratio)));
    
    // Track maximum reduction
    if (det->current_gr < det->max_gr) {
        det->max_gr = det->current_gr;
    }
    
    // Convert reduction to linear gain
    return powf(10.0f, det->current_gr * 0.05f);
}