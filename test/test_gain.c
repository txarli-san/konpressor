#include "../src/level_detector.h"
#include <stdio.h>
#include <math.h>

int main() {
    // Test 1: Unity gain signal
    float unity_signal[32];
    for(int i = 0; i < 32; i++) {
        unity_signal[i] = 1.0f;
    }
    
    // Test 2: -6dB signal (0.5 amplitude)
    float minus6db_signal[32];
    for(int i = 0; i < 32; i++) {
        minus6db_signal[i] = 0.5f;
    }
    
    LevelDetector det;
    init_level_detector(&det, -20.0f, 4.0f, 10.0f, 100.0f, 6.0f);
    
    // Process and print each stage
    printf("Unity Signal Test:\n");
    float gain = process_level_detector(&det, unity_signal, 32);
    printf("Level: %.1f dB\n", det.current_level);
    printf("GR: %.1f dB\n", det.current_gr);
    printf("Gain: %.6f\n", gain);
    
    printf("\n-6dB Signal Test:\n");
    gain = process_level_detector(&det, minus6db_signal, 32);
    printf("Level: %.1f dB\n", det.current_level);
    printf("GR: %.1f dB\n", det.current_gr);
    printf("Gain: %.6f\n", gain);
    
    return 0;
}