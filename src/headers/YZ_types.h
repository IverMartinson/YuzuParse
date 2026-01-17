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
    float current_sample;
    float* pcm_data;
    uint32_t channel_count;
    uint32_t sample_count;
    double* pitch_multiplier;
    double* speed_multiplier;
    unsigned char* should_loop_audio;
    void(*end_function)(void*);
    void* custom_pointer_for_end_function;
} pa_callback_data;

typedef struct {
    float left_tone;
    float right_tone;
    float left_amplitude;
    float right_amplitude;
    float (*tone_generator)(float, float);
    uint32_t current_sample;
    uint32_t sample_count;
    uint32_t sample_rate;
    float duration;
    float value; // a value that is passed into the tone generator. could be HZ for example
} pa_tone_callback_data;

#endif