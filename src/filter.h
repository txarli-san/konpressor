#ifndef FILTER_H
#define FILTER_H

typedef struct {
    float freq;
    float a1, a2;
    float b0, b1, b2;
    float x1, x2;
    float y1, y2;
} BiquadFilter;

void init_biquad(BiquadFilter *f, float freq);
float process_biquad(BiquadFilter *f, float in);

#endif