#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define TEST_DURATION 0.1
#define NUM_SAMPLES (int)(SAMPLE_RATE * TEST_DURATION)

typedef struct {
    float freq;
    float a0, a1;
    float x1, y1;
} HighPassFilter;

void init_filter(HighPassFilter *f, float freq) {
    float w0 = 2.0f * M_PI * freq / SAMPLE_RATE;
    float alpha = (1.0f - w0) / (1.0f + w0);
    
    f->freq = freq;
    f->a0 = (1.0f + alpha) / 2.0f;
    f->a1 = -(1.0f + alpha) / 2.0f;
    f->x1 = 0.0f;
    f->y1 = 0.0f;
}

float process_sample(HighPassFilter *f, float in) {
    float out = f->a0 * in + f->a1 * f->x1 - f->a0 * f->y1;
    f->x1 = in;
    f->y1 = out;
    return out;
}

float generate_sine(float freq, int sample) {
    return sinf(2.0f * M_PI * freq * sample / SAMPLE_RATE);
}

int main() {
    HighPassFilter filter_low, filter_high;
    init_filter(&filter_low, 100.0f);
    init_filter(&filter_high, 100.0f);

    printf("Testing with 50 Hz (below cutoff) and 200 Hz (above cutoff):\n");
    
    for(int i = 0; i < 100; i += 10) {
        float in_low = generate_sine(50.0f, i);
        float in_high = generate_sine(200.0f, i);
        
        float out_low = process_sample(&filter_low, in_low);
        float out_high = process_sample(&filter_high, in_high);
        
        printf("Sample %d:\n", i);
        printf("  50Hz:  In=%.3f Out=%.3f\n", in_low, out_low);
        printf("  200Hz: In=%.3f Out=%.3f\n", in_high, out_high);
    }

    return 0;
}