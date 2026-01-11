#include "../headers/yuzuparse.h"

int main(){
    YZ_audio_stream* audio_stream = YZ_load_audio_file("audio/voice.wav", 0);

    pa_callback_data* callback_data;

    if (audio_stream)
        callback_data = YZ_play_stream(audio_stream);

    // wait until audio is finished playing until exiting
    while (callback_data->current_sample < callback_data->sample_count){}

    YZ_kill_player();

    return 0;
}
