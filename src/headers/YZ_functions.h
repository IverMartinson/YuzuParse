#ifndef YZ_FUNCTIONS_H
#define YZ_FUNCTIONS_H

#include "YZ_types.h"

// loads an audio file into a playable format
YZ_audio_stream* YZ_load_audio_file(char* filename, unsigned char debug_mode);

// play an audio stream one time though
pa_callback_data* YZ_play_stream(YZ_audio_stream* audio_stream);

// play an audio stream with optional effects. You can change speed, turn on looping, or set the starting second of the stream.
pa_callback_data* YZ_play_stream_dynamic(YZ_audio_stream* audio_stream, double* speed_multiplier, unsigned char* should_loop_audio, double start_second, void (*end_function)(void*), void* custom_pointer_for_end_function);

// Initializes the audio player. Playing a stream when the player is uninitializes automatically runs this function.
void YZ_init_player();

// Kills the player and frees memory
void YZ_kill_player();

#endif