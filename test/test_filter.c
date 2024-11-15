#include "../src/filter.h"
#include "test_utils.h"
#include <stdio.h>

int main() {
    BiquadFilter filter_low, filter_high;
    init_biquad(&filter_low, 100.0f);
    init_biquad(&filter_high, 100.0f);
    
    printf("Sample,50Hz_In,50Hz_Out,200Hz_In,200Hz_Out\n");
    
    for(int i = 0; i < 100; i++) {
        float in_low = generate_sine(50.0f, i);
        float in_high = generate_sine(200.0f, i);
        
        float out_low = process_biquad(&filter_low, in_low);
        float out_high = process_biquad(&filter_high, in_high);
        
        printf("%d,%.3f,%.3f,%.3f,%.3f\n", 
               i, in_low, out_low, in_high, out_high);
    }
    
    return 0;
}