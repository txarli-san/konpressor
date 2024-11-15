#include "processor.h"
#include <math.h>

void init_processor(AudioProcessor *proc) {
    proc->freq = 100.0f;
    proc->threshold = 0.0f;
    proc->attack_ms = 10.0f;
    proc->release_ms = 100.0f;
    proc->level = 1.0f;
    proc->pre_post_enabled = 0;
    
    init_biquad(&proc->freq_filter, proc->freq);
    init_bypass(&proc->bypass);
    init_level_detector(&proc->detector, proc->attack_ms, proc->release_ms);
    
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
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc->bypass.input_a[i] = proc->in_a[i];
        proc->bypass.input_b[i] = proc->in_b[i];
    }
    
    process_bypass(&proc->bypass);
    
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc->filtered[i] = process_biquad(&proc->freq_filter, proc->in_a[i]);
    }
    
    process_level_detector(&proc->detector, proc->filtered, BLOCK_SIZE);
    
    float gain = 1.0f;
    if(proc->detector.envelope > proc->threshold && proc->threshold > 0.0f) {
        gain = proc->threshold / proc->detector.envelope;
    }
    
    for(int i = 0; i < BLOCK_SIZE; i++) {
        if(proc->bypass.bypass_enabled) {
            proc->out[i] = proc->bypass.output[i];
        } else {
            proc->out[i] = proc->filtered[i] * gain * proc->level;
        }
    }
}