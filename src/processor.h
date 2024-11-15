#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "config.h"
#include "filter.h"
#include "bypass.h"

typedef struct {
    // Input/Output buffers
    float in_a[BLOCK_SIZE];
    float in_b[BLOCK_SIZE];
    float filtered[BLOCK_SIZE];
    float out[BLOCK_SIZE];
    
    // Processing modules
    BiquadFilter freq_filter;
    BypassModule bypass;
    
    // Parameters
    float freq;
    float threshold;
    float attack_ms;
    float release_ms;
    float level;
    
    // Routing
    int pre_post_enabled;
} AudioProcessor;

void init_processor(AudioProcessor *proc);
void set_frequency(AudioProcessor *proc, float freq);
void set_bypass(AudioProcessor *proc, int enabled);
void set_pre_post(AudioProcessor *proc, int enabled);
void process_block(AudioProcessor *proc);

#endif
