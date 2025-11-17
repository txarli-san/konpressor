#ifndef HATE_H
#define HATE_H

typedef struct {
    float drive;        // Saturation drive (1.0 = no saturation)
    float pre_post_mix; // Wet/dry mix (0.0 = dry, 1.0 = wet)
    int bypass;         // Bypass flag
} HateModule;

void init_hate(HateModule *hate);
void process_hate(HateModule *hate, const float *input, float *output, int size);

#endif