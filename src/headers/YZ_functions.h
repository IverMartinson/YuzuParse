#ifndef YZ_FUNCTIONS_H
#define YZ_FUNCTIONS_H

#include "YZ_types.h"

YZ_audio_stream* YZ_load_audio_file(char* filename, unsigned char debug_mode);
pa_callback_data* YZ_play_stream(YZ_audio_stream* audio_stream);
pa_callback_data* YZ_play_stream_dynamic(YZ_audio_stream* audio_stream, double* speed_multiplier, unsigned char* should_loop_audio);
void YZ_init_player();
void YZ_kill_player();

#endif