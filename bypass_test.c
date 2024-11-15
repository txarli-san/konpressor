#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SAMPLE_RATE 44100
#define DURATION_SEC 1
#define NUM_SAMPLES (SAMPLE_RATE * DURATION_SEC)

typedef struct {
    float input_a[NUM_SAMPLES];
    float input_b[NUM_SAMPLES];
    float output[NUM_SAMPLES];
    int bypass_enabled;
} BypassModule;

void generate_test_signal(float *buffer, float freq) {
    for(int i = 0; i < NUM_SAMPLES; i++) {
        buffer[i] = sinf(2.0f * M_PI * freq * i / SAMPLE_RATE);
    }
}

void process_bypass(BypassModule *bypass) {
    for(int i = 0; i < NUM_SAMPLES; i++) {
        bypass->output[i] = bypass->bypass_enabled ? 
            bypass->input_a[i] + bypass->input_b[i] : 0.0f;
    }
}

int main() {
    BypassModule bypass = {0};
    
    generate_test_signal(bypass.input_a, 440.0f);
    generate_test_signal(bypass.input_b, 880.0f);
    
    printf("Testing bypass enabled:\n");
    bypass.bypass_enabled = 1;
    process_bypass(&bypass);
    for(int i = 0; i < 5; i++) {
        printf("Sample %d: A=%.3f B=%.3f Out=%.3f\n", 
               i, bypass.input_a[i], bypass.input_b[i], bypass.output[i]);
    }
    
    printf("\nTesting bypass disabled:\n");
    bypass.bypass_enabled = 0;
    process_bypass(&bypass);
    for(int i = 0; i < 5; i++) {
        printf("Sample %d: A=%.3f B=%.3f Out=%.3f\n", 
               i, bypass.input_a[i], bypass.input_b[i], bypass.output[i]);
    }
    
    return 0;
}