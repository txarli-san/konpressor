#ifndef BYPASS_H
#define BYPASS_H

#include "config.h"

typedef struct {
    float input_a[BLOCK_SIZE];
    float input_b[BLOCK_SIZE];
    float output[BLOCK_SIZE];
    int bypass_enabled;
} BypassModule;

void init_bypass(BypassModule *bypass);
void process_bypass(BypassModule *bypass);

#endif