#include "filter.h"
#include "config.h"
#include <math.h>

void init_biquad(BiquadFilter *f, float freq) {
    float omega = 2.0f * M_PI * freq / SAMPLE_RATE;
    float alpha = sinf(omega) / (2.0f * 0.707f);
    float cosw = cosf(omega);
    
    float norm = 1.0f / (1.0f + alpha);
    
    f->freq = freq;
    f->b0 = (1.0f + cosw) * 0.5f * norm;
    f->b1 = -(1.0f + cosw) * norm;
    f->b2 = f->b0;
    f->a1 = -2.0f * cosw * norm;
    f->a2 = (1.0f - alpha) * norm;
    
    f->x1 = f->x2 = f->y1 = f->y2 = 0.0f;
}

float process_biquad(BiquadFilter *f, float in) {
    float out = f->b0 * in + f->b1 * f->x1 + f->b2 * f->x2 
              - f->a1 * f->y1 - f->a2 * f->y2;
    
    f->x2 = f->x1;
    f->x1 = in;
    f->y2 = f->y1;
    f->y1 = out;
    
    return out;
}