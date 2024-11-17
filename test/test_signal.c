#include "../src/processor.h"
#include "test_utils.h"
#include <stdio.h>
#include <math.h>

static float compute_rms_level(const float *buffer, int size) {
    float sum_squared = 0.0f;
    for(int i = 0; i < size; i++) {
        sum_squared += buffer[i] * buffer[i];
    }
    return 20.0f * log10f(sqrtf(sum_squared / size));
}

int main() {
    float buffer[BLOCK_SIZE];
    
    // Test exact dB levels
    float test_levels[] = {0.0f, -6.0f, -12.0f, -20.0f};
    
    for(int test = 0; test < 4; test++) {
        float target_db = test_levels[test];
        
        for(int i = 0; i < BLOCK_SIZE; i++) {
            buffer[i] = generate_sine_at_db(440.0f, i, target_db);
        }
        
        float measured_db = compute_rms_level(buffer, BLOCK_SIZE);
        printf("Target: %.1f dB, Measured: %.1f dB, Error: %.2f dB\n", 
               target_db, measured_db, measured_db - target_db);
    }
    
    return 0;
}