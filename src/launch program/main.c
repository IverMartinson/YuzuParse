#include "../headers/yuzuparse.h"

float saw_wave_generator(float time, float value){
    // play a VALUE hz note
    
    return fmodf(time * value * 2 + 1.0, 2.0) - 1.0;
}

float sine_wave_generator(float time, float value){
    // play a VALUE hz note
    
    return sin(time * value * 3.14159264 * 2);
}

float square_wave_generator(float time, float value){
    // play a VALUE hz note
    
    return fmodf(time * value, 1) - fmodf(time * value + 0.5, 1);
}

float triangle_wave_generator(float time, float value){
    // play a VALUE hz note
    
    return sin(time * value * 3.14159264 * 2);
}

int main(){
    pa_tone_callback_data* data;

    data = YZ_play_tone(sine_wave_generator, 44100, 100, 220);
    data = YZ_play_tone(sine_wave_generator, 44100, 100, 440);
    data = YZ_play_tone(sine_wave_generator, 44100, 100, 880);
    data = YZ_play_tone(sine_wave_generator, 44100, 100, 110);

    while (data->current_sample < data->sample_count){
    }

    YZ_kill_player();

    return 0;
}
