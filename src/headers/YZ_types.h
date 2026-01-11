#ifndef YZ_TYPES_H
#define YZ_TYPES_H

#include <stdint.h>

typedef struct {
    float* pcm_data;
    double sample_rate; // samples per second
    uint8_t channel_count;
    uint32_t sample_count;
} YZ_audio_stream;

typedef struct {
    uint32_t current_sample;
    float* pcm_data;
    uint32_t channel_count;
    uint32_t sample_count;
} pa_callback_data;

#endif