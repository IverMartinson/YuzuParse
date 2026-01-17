#include "../headers/yuzuparse.h"

int (*cprintf)(const char *__restrict __format, ...) = printf;

int printf_override(const char *__restrict __format, ...){
    return 0;
}

#define print (*cprintf)

char* lsb_byte_to_binary(uint32_t byte, uint8_t bits){
    char* string = malloc(sizeof(char) * (bits + 1));

    for (int i = 0; i < bits; i++){
        string[i] = ((byte >> i) & 1) + 0x30;
    }

    string[bits] = '\0';

    return string; 
}

// MSB, most sig bit is first
// 1 = 00000001
char* msb_byte_to_binary(uint32_t byte, uint8_t bits){
    char* string = malloc(sizeof(char) * (bits + 1));

    for (int i = 0; i < bits; i++){
        string[bits - 1 - i] = ((byte >> i) & 1) + 0x30;
    }

    string[bits] = '\0';

    return string;
}

// MSB, most sig bit is first
// 1 = 00000001
uint16_t binary_to_int(char* binary, uint8_t number_of_bits){
    uint16_t final_value = 0;

    for (int i = 0; i < number_of_bits; i++){
        final_value += (1 << i) * (binary[number_of_bits - i - 1] - 0x30);
    }

    return final_value;
}

// return individial bit's value. Zero indexed
// msb_get_bit(00100, 2) == 1
uint8_t msb_get_bit(uint32_t data, uint8_t bit){
    return (data >> bit) & 1;
}

uint32_t current_byte = 0;
uint8_t* file_buffer = NULL;

// byte reading funcs

void skip(int bytes_to_skip){
    current_byte += bytes_to_skip;
}

int8_t get_1(){
    return file_buffer[current_byte++];
}

int16_t get_2(){
    int16_t result = file_buffer[current_byte] | (file_buffer[current_byte + 1] << 8);
    
    current_byte += 2;
    
    return result;
}

int32_t get_4(){
    int32_t result = file_buffer[current_byte] | 
        (file_buffer[current_byte + 1] << 8) | 
        (file_buffer[current_byte + 2] << 16) | 
        (file_buffer[current_byte + 3] << 24);
    
    current_byte += 4;

    return result;
}

YZ_audio_stream* YZ_load_wav(){
    YZ_audio_stream* audio_stream = malloc(sizeof(YZ_audio_stream));

    char filename[5] = {get_1(), get_1(), get_1(), get_1(), '\0'};

    if (strcmp(filename, "RIFF")){
        printf("file is not a WAVE file\n");

        return NULL;
    }

    uint32_t chunk_size = get_4();
    char wave_id[5] = {get_1(), get_1(), get_1(), get_1(), '\0'};

    unsigned char reading_file = 1;

    uint32_t chunk_id;

    uint16_t bits_per_sample;
    uint16_t data_block_size;

    skip(4);

    print("reading format chunk at byte #%d\n", current_byte - 4);

    uint32_t fmt_chunk_size = get_4();
    uint16_t format_code = get_2();
    uint16_t interleaved_channel_count = get_2();
    uint32_t samples_per_second = get_4(); // blocks per second
    uint32_t bytes_per_second = get_4();
    data_block_size = get_2();
    bits_per_sample = get_2();
    uint16_t extension_size = 0;
    if (fmt_chunk_size > 16) extension_size = get_2();
    uint16_t valid_bits_per_sample = 0;
    if (fmt_chunk_size > 18) valid_bits_per_sample = get_2();

    if (fmt_chunk_size != 16){
        printf("audio data is not PCM. cannot read this yet\n");

        return NULL;
    }

    if (format_code != 1){
        printf("audio data is compressed. cannot read this yet\n");

        return NULL;
    }

    audio_stream->channel_count = interleaved_channel_count;
    audio_stream->sample_rate = (double)samples_per_second;

    print("fmt_chunk_size: %d\nformat_code: %d\ninterleaved_channel_count: %d\nsamples_per_second: %d\nbytes_per_second: %d\ndata_block_size: %d\nbits_per_sample: %d\nextension_size: %d\nvalid_bits_per_sample: %d\n", fmt_chunk_size, format_code, interleaved_channel_count, samples_per_second, bytes_per_second, data_block_size, bits_per_sample, extension_size, valid_bits_per_sample);

    // data

    while (get_4() != 1635017060) 
        current_byte -= 3;

    print("reading data chunk at byte #%d\n", current_byte - 4);

    uint32_t data_chunk_size = get_4();
    
    audio_stream->pcm_data = malloc(sizeof(double) * (data_chunk_size / (bits_per_sample / 8)));

    audio_stream->sample_count = data_chunk_size / (bits_per_sample / 8) / audio_stream->channel_count;

    print("data chunk is %d bytes\n", data_chunk_size);
    
    uint32_t current_sample = 0;

    for (uint32_t i = 0; i < data_chunk_size;){
        uint32_t sample_index = current_sample++ * audio_stream->channel_count;
        
        uint32_t bytes_read = 0;

        for (uint32_t j = 0; j < audio_stream->channel_count; j++){
            double value;

            if (bits_per_sample == 8){
                value = ((double)get_1() - 128.0) / 128.0;
                bytes_read += 1;
            } else if (bits_per_sample == 16){
                value = (double)(int16_t)get_2() / 32768.0;
                bytes_read += 2;
            } else if (bits_per_sample == 32){
                printf("cannot read 32 bit per sample audio yet\n");

                return NULL;
            }

            audio_stream->pcm_data[sample_index + j] = value;
        }

        i += bytes_read;
    }

    return audio_stream;
}

YZ_audio_stream* YZ_load_mp3(){return NULL;}
YZ_audio_stream* YZ_load_ogg(){return NULL;}
YZ_audio_stream* YZ_load_flac(){return NULL;}
YZ_audio_stream* YZ_load_aiff(){return NULL;}

YZ_audio_stream* YZ_load_audio_file(char* filename, unsigned char debug_mode){
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL){
        printf("file \"%s\" does not exist !!!\n", filename);

        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    file_buffer = malloc(size);

    fread(file_buffer, 1, size, fp);
    fclose(fp);

    // override (*cprintf) so there is no debug output
    if (!debug_mode){
        cprintf = printf_override;
    }

    // see what the file type is

    char* mutable_filename = malloc(sizeof(char) * (strlen(filename) + 1));
    strcpy(mutable_filename, filename);

    char *strtok_string = strtok(mutable_filename, ".");
    char *filetype_string;
    
    while(strtok_string != NULL) {
        filetype_string = strtok_string;
        strtok_string = strtok(NULL, ".");
    }
    
    current_byte = 0;
    
    if (!strcmp(filetype_string, "wav") || !strcmp(filetype_string, "WAV") || !strcmp(filetype_string, "wave") || !strcmp(filetype_string, "WAVE")){ 
        return YZ_load_wav();
    } else 
    if (!strcmp(filetype_string, "mp3") || !strcmp(filetype_string, "MP3") || !strcmp(filetype_string, "mpga") || !strcmp(filetype_string, "MPGA")){ 
        return YZ_load_mp3();
    } else 
    if (!strcmp(filetype_string, "placeholder") || !strcmp(filetype_string, "placeholder")){ 
        return YZ_load_ogg();
    } else 
    if (!strcmp(filetype_string, "placeholder") || !strcmp(filetype_string, "placeholder")){ 
        return YZ_load_flac();
    } else 
    if (!strcmp(filetype_string, "placeholder") || !strcmp(filetype_string, "placeholder")){ 
        return YZ_load_aiff();
    }

    (*cprintf)("file is unreadable by Yuzu\n");

    return NULL;
}

int portaudio_callback(const void* input, void* output, unsigned long frame_count, const PaStreamCallbackTimeInfo* pa_time_info, PaStreamCallbackFlags pa_status_flags, void* audio_data){
    pa_callback_data* data = (pa_callback_data*)audio_data;
    
    float* out = (float*)output;

    for(uint32_t i = 0; i < frame_count; i++){
        for (uint32_t j = 0; j < data->channel_count; j++) {
            *out++ = data->pcm_data[(int)(data->current_sample) * data->channel_count + j];
        }

        data->current_sample += *data->speed_multiplier;
    
        if (*data->should_loop_audio){
            if (data->current_sample >= data->sample_count){
                data->current_sample = data->sample_count - data->current_sample;
            } else if (data->current_sample < 0){
                data->current_sample = data->sample_count + data->current_sample;
            }
        }

        if (data->current_sample >= data->sample_count || data->current_sample < 0){
            goto END;
        }
    }

    if (data->current_sample >= data->sample_count){
        goto END;
    }

    return 0;

    END:
        if (data->end_function) data->end_function(data->custom_pointer_for_end_function);

        free(audio_data);

        return 1;    
}

int tone_callback(const void* input, void* output, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo* time_info, PaStreamFlags status_flags, void* user_data){
    pa_tone_callback_data* data = (pa_tone_callback_data*)user_data;
    float *out = (float*)output;
    unsigned long i;

    for(i = 0; i < frames_per_buffer; i++){
        *out++ = data->left_tone;
        *out++ = data->right_tone;
        
        float delta = data->tone_generator((double)data->current_sample / (double)data->sample_rate, data->value);

        data->left_tone = delta;
        if(data->left_tone >= 1.0) data->left_tone = 1.0;
        if(data->left_tone <= -1.0) data->left_tone = -1.0;
        
        data->right_tone = delta;
        if(data->right_tone >= 1.0) data->right_tone = 1.0;
        if(data->right_tone <= -1.0) data->right_tone = -1.0;
    
        data->current_sample++;
    }

    if (data->current_sample >= data->sample_count){
        return 1;
    }

    return 0;
}

unsigned char player_is_initialized = 0;

void YZ_init_player(){
    if (player_is_initialized){
        printf("cannot init, already started");
    
        return;
    }
    
    PaError error;

    // block PortAudio's standard output and standard error stream
    freopen("/dev/null", "w", stdout); 
    freopen("/dev/null", "w", stderr); 

    error = Pa_Initialize();
    
    // unblock PortAudio's standard output and standard error stream
    freopen("/dev/tty", "w", stdout);
    freopen("/dev/tty", "w", stderr);

    if(error != paNoError) { printf("PortAudio error: %d\n", error); exit(1);}
    
    player_is_initialized = 1;
}

void YZ_kill_player(){
    if (!player_is_initialized){
        printf("cannot kill, player isn't initilized");

        return;
    }

    PaError error;

    error = Pa_Terminate();
    if(error != paNoError) { printf("PortAudio error: %d\n", error); exit(1);}
}

pa_callback_data* play_stream(YZ_audio_stream* audio_stream, double* pitch_multiplier, double* speed_multiplier, unsigned char* should_loop_audio, double starting_second, void (*end_func)(void* custom_pointer_for_end_function), void* custom_pointer_for_end_function){
    PaError error;
    
    PaStream *stream;

    pa_callback_data* callback_data = malloc(sizeof(pa_callback_data));

    callback_data->channel_count = audio_stream->channel_count;
    callback_data->current_sample = starting_second >= 0 ? audio_stream->sample_rate * starting_second : audio_stream->sample_count;
    callback_data->pcm_data = audio_stream->pcm_data;
    callback_data->sample_count = audio_stream->sample_count;
    callback_data->pitch_multiplier = pitch_multiplier;
    callback_data->speed_multiplier = speed_multiplier;
    callback_data->should_loop_audio = should_loop_audio;
    callback_data->end_function = end_func;
    callback_data->custom_pointer_for_end_function = custom_pointer_for_end_function;

    if (!player_is_initialized) YZ_init_player();

    error = Pa_OpenDefaultStream(&stream, 0, audio_stream->channel_count, paFloat32, audio_stream->sample_rate, paFramesPerBufferUnspecified, portaudio_callback, callback_data);
    if(error != paNoError) { printf("PortAudio error: %d\n", error); exit(1);}

    Pa_StartStream(stream);

    return callback_data;
}

// play an audio stream
pa_callback_data* YZ_play_stream(YZ_audio_stream* audio_stream){
    double value = 1;
    unsigned char antivalue = 0;

    return play_stream(audio_stream, &value, &value, &antivalue, (double)antivalue, NULL, NULL);
}

// play an audio stream and modify pitch and speed dynamically. Pitch and speed changes work in real time due to it being pointers to the values
pa_callback_data* YZ_play_stream_dynamic(YZ_audio_stream* audio_stream, double* speed_multiplier, unsigned char* should_loop_audio, double starting_second, void (*end_func)(void*), void* custom_pointer_for_end_function){
    return play_stream(audio_stream, NULL, speed_multiplier, should_loop_audio, starting_second, end_func, custom_pointer_for_end_function);
}

pa_tone_callback_data* YZ_play_tone(float (*tone_generator)(float, float), float sample_rate, float duration, float value){
    if (!player_is_initialized) YZ_init_player();

    pa_tone_callback_data* callback_data = malloc(sizeof(pa_tone_callback_data));

    callback_data->tone_generator = tone_generator;
    callback_data->current_sample = 0;
    callback_data->duration = duration;
    callback_data->left_amplitude = 1;
    callback_data->right_amplitude = 1;
    callback_data->sample_rate = sample_rate;
    callback_data->sample_count = duration * sample_rate;
    callback_data->value = value;

    PaStream* stream;

    PaError error = Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, sample_rate, paFramesPerBufferUnspecified, tone_callback, callback_data);
    if(error != paNoError) { printf("PortAudio error: %d\n", error); exit(1);}

    Pa_StartStream(stream);

    return callback_data;
}
