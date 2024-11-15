#include "bypass.h"

void init_bypass(BypassModule *bypass) {
    bypass->bypass_enabled = 0;
    for(int i = 0; i < BLOCK_SIZE; i++) {
        bypass->input_a[i] = 0.0f;
        bypass->input_b[i] = 0.0f;
        bypass->output[i] = 0.0f;
    }
}

void process_bypass(BypassModule *bypass) {
    for(int i = 0; i < BLOCK_SIZE; i++) {
        bypass->output[i] = bypass->bypass_enabled ? 
            bypass->input_a[i] + bypass->input_b[i] : 0.0f;
    }
}
