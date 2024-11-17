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
    // Calculate RMS
    float sum_squared = 0.0f;
    for (int i = 0; i < nframes; i++) {
        sum_squared += input[i] * input[i];
    }
    float rms = sqrtf(sum_squared / nframes);
    float input_db = 20.0f * log10f(rms + 1e-30f);
    
    det->current_level = input_db;
    
    // If below threshold, no processing
    if (input_db <= det->threshold) {
        det->current_gr = 0.0f;
        return 1.0f;
    }
    
    // Calculate how far input is above threshold
    float db_over = input_db - det->threshold;
    
    // For a ratio of R:1, input should only rise by 1dB for every R dB over threshold
    float output_db_over = db_over / det->ratio;
    float target_db = det->threshold + output_db_over;
    
    // Calculate gain needed
    float gain_db = target_db - input_db;
    det->current_gr = gain_db;
    
    // Convert to linear gain
    return powf(10.0f, gain_db / 20.0f);
}
// float process_level_detector(LevelDetector *det, const float *input, int nframes) {
//     // Print first few samples to verify input
//     printf("First 5 samples: ");
//     for (int i = 0; i < 5; i++) {
//         printf("%.6f ", input[i]);
//     }
//     printf("\n");

//     // Calculate RMS
//     float sum_squared = 0.0f;
//     for (int i = 0; i < nframes; i++) {
//         sum_squared += input[i] * input[i];
//     }
//     float rms = sqrtf(sum_squared / nframes);
//     float rms_db = 20.0f * log10f(rms + 1e-30f);
    
//     printf("RMS: %.6f (%.1f dB)\n", rms, rms_db);
    
//     det->current_level = rms_db;
    
//     if (rms_db <= det->threshold) {
//         det->current_gr = 0.0f;
//         return 1.0f;
//     }
    
//     float over = rms_db - det->threshold;
//     float reduction = over * (1.0f - 1.0f/det->ratio);
//     det->current_gr = -reduction;
    
//     return powf(10.0f, -reduction / 20.0f);
// }