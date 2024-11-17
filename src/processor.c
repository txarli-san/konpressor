#include "processor.h"
#include <math.h>

void init_processor(AudioProcessor *proc) {
    // Initialize with conservative defaults
    proc->freq = 100.0f;
    proc->threshold = 0.0f;      // 0 dB threshold
    proc->ratio = 2.0f;         // 2:1 compression
    proc->attack_ms = 10.0f;    
    proc->release_ms = 100.0f;  
    proc->knee_width = 6.0f;    
    proc->level = 1.0f;
    
    proc->pre_post_enabled = 0;
    proc->sc_enabled = 0;
    
    // Initialize modules
    init_biquad(&proc->freq_filter, proc->freq);
    init_biquad(&proc->sc_filter, proc->freq);
    init_bypass(&proc->bypass);
    init_level_detector(&proc->detector, 
                       proc->threshold,
                       proc->ratio,
                       proc->attack_ms,
                       proc->release_ms,
                       proc->knee_width);
    
    // Clear buffers
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc->in_a[i] = 0.0f;
        proc->in_b[i] = 0.0f;
        proc->filtered[i] = 0.0f;
        proc->sc_filtered[i] = 0.0f;
        proc->out[i] = 0.0f;
    }
    
    proc->current_gr = 0.0f;
    proc->max_gr = 0.0f;
}

void set_threshold(AudioProcessor *proc, float threshold) {
    proc->threshold = threshold;
    update_detector(&proc->detector, proc->threshold, proc->ratio,
                   proc->attack_ms, proc->release_ms, proc->knee_width);
}

void set_ratio(AudioProcessor *proc, float ratio) {
    proc->ratio = ratio;
    update_detector(&proc->detector, proc->threshold, proc->ratio,
                   proc->attack_ms, proc->release_ms, proc->knee_width);
}

void set_attack(AudioProcessor *proc, float attack_ms) {
    proc->attack_ms = attack_ms;
    update_detector(&proc->detector, proc->threshold, proc->ratio,
                   proc->attack_ms, proc->release_ms, proc->knee_width);
}

void set_release(AudioProcessor *proc, float release_ms) {
    proc->release_ms = release_ms;
    update_detector(&proc->detector, proc->threshold, proc->ratio,
                   proc->attack_ms, proc->release_ms, proc->knee_width);
}

void set_knee(AudioProcessor *proc, float knee_width) {
    proc->knee_width = knee_width;
    update_detector(&proc->detector, proc->threshold, proc->ratio,
                   proc->attack_ms, proc->release_ms, proc->knee_width);
}

void set_level(AudioProcessor *proc, float level) {
    proc->level = level;
}

void set_frequency(AudioProcessor *proc, float freq) {
    proc->freq = freq;
    init_biquad(&proc->freq_filter, freq);
    init_biquad(&proc->sc_filter, freq);
}

void set_bypass(AudioProcessor *proc, int enabled) {
    proc->bypass.bypass_enabled = enabled;
}

void set_pre_post(AudioProcessor *proc, int enabled) {
    proc->pre_post_enabled = enabled;
}

void set_sidechain(AudioProcessor *proc, int enabled) {
    proc->sc_enabled = enabled;
}

void process_block(AudioProcessor *proc) {
    if(proc->bypass.bypass_enabled) {
        for(int i = 0; i < BLOCK_SIZE; i++) {
            proc->out[i] = proc->in_a[i];
        }
        return;
    }
    
    // Filter main signal
    for(int i = 0; i < BLOCK_SIZE; i++) {
        proc->filtered[i] = process_biquad(&proc->freq_filter, proc->in_a[i]);
    }
    
    // Process sidechain or main signal for detection
    const float *detect_signal;
    if(proc->sc_enabled) {
        for(int i = 0; i < BLOCK_SIZE; i++) {
            proc->sc_filtered[i] = process_biquad(&proc->sc_filter, proc->in_b[i]);
        }
        detect_signal = proc->sc_filtered;
    } else {
        detect_signal = proc->filtered;
    }
    
    // Get gain reduction and apply it, preserving signal polarity
    float gain = process_level_detector(&proc->detector, detect_signal, BLOCK_SIZE);
    
    proc->current_gr = proc->detector.current_gr;
    proc->max_gr = proc->detector.max_gr;
    
    for(int i = 0; i < BLOCK_SIZE; i++) {
        // Preserve signal polarity by applying gain directly
        proc->out[i] = proc->filtered[i] * gain;
    }
}