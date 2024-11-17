#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "config.h"
#include "filter.h"
#include "bypass.h"
#include "level_detector.h"

typedef struct {
    // Input/Output buffers
    float in_a[BLOCK_SIZE];
    float in_b[BLOCK_SIZE];
    float filtered[BLOCK_SIZE];
    float out[BLOCK_SIZE];
    float sc_filtered[BLOCK_SIZE];  // Sidechain filtered buffer
    
    // Processing modules
    BiquadFilter freq_filter;    // Main filter
    BiquadFilter sc_filter;      // Sidechain filter
    BypassModule bypass;
    LevelDetector detector;
    
    // Parameters
    float freq;          // Filter frequency
    float threshold;     // Compression threshold (dB)
    float ratio;         // Compression ratio
    float attack_ms;     // Attack time (ms)
    float release_ms;    // Release time (ms)
    float knee_width;    // Soft knee width (dB)
    float level;         // Output level
    
    // Routing
    int pre_post_enabled;
    int sc_enabled;      // Sidechain enable
    
    // Metering
    float current_gr;    // Current gain reduction
    float max_gr;        // Maximum gain reduction
} AudioProcessor;

void init_processor(AudioProcessor *proc);
void set_frequency(AudioProcessor *proc, float freq);
void set_threshold(AudioProcessor *proc, float threshold);
void set_ratio(AudioProcessor *proc, float ratio);
void set_attack(AudioProcessor *proc, float attack_ms);
void set_release(AudioProcessor *proc, float release_ms);
void set_knee(AudioProcessor *proc, float knee_width);
void set_level(AudioProcessor *proc, float level);
void set_bypass(AudioProcessor *proc, int enabled);
void set_pre_post(AudioProcessor *proc, int enabled);
void set_sidechain(AudioProcessor *proc, int enabled);
void process_block(AudioProcessor *proc);

#endif