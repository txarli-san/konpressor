#include "hate.h"
#include <math.h>

void init_hate(HateModule *hate) {
    hate->drive = 1.0f;        // No saturation by default
    hate->pre_post_mix = 0.0f; // Dry signal by default
    hate->bypass = 0;          // Not bypassed
}

void process_hate(HateModule *hate, const float *input, float *output, int size) {
    if (hate->bypass) {
        for (int i = 0; i < size; i++) {
            output[i] = input[i];
        }
        return;
    }

    for (int i = 0; i < size; i++) {
        // Apply saturation using tanh for soft clipping
        float saturated = tanhf(input[i] * hate->drive);

        // Wet/dry mix
        output[i] = input[i] * (1.0f - hate->pre_post_mix) + saturated * hate->pre_post_mix;
    }
}