#include "processor.h"

void init_processor(AudioProcessor *proc) {
    // Initialize parameters
    proc->freq = 100.0f;
    proc->threshold = 0.0f;
    proc->attack_ms = 10.0f;
    proc->release_ms = 100.0f;
    proc->level = 1.0f;
    proc->pre_post_enabled = 0;
    
    // Initialize modules
    init_biquad(&proc->freq_filter, proc->freq);
    init_bypass(&proc->bypass);
    
    // Clear buffers
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc->in_a[i] = 0.0f;
        proc->in_b[i] = 0.0f;
        proc->filtered[i] = 0.0f;
        proc->out[i] = 0.0f;
    }
}

void set_frequency(AudioProcessor *proc, float freq) {
    proc->freq = freq;
    init_biquad(&proc->freq_filter, freq);
}

void set_bypass(AudioProcessor *proc, int enabled) {
    proc->bypass.bypass_enabled = enabled;
}

void set_pre_post(AudioProcessor *proc, int enabled) {
    proc->pre_post_enabled = enabled;
}

void process_block(AudioProcessor *proc) {
    // Copy inputs to bypass module
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc->bypass.input_a[i] = proc->in_a[i];
        proc->bypass.input_b[i] = proc->in_b[i];
    }
    
    // Process bypass
    process_bypass(&proc->bypass);
    
    // Filter processing - currently just channel A
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc->filtered[i] = process_biquad(&proc->freq_filter, proc->in_a[i]);
    }
    
    // Output routing
    for(int i = 0; i < BLOCK_SIZE; i++) {
        if(proc->bypass.bypass_enabled) {
            proc->out[i] = proc->bypass.output[i];
        } else {
            proc->out[i] = proc->filtered[i];
        }
    }
}