#ifndef AUDIO_PLAYER_HPP
#define AUDIO_PLAYER_HPP

#include "wav_file/wav_file.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2s_std.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

class AudioPlayer {
    public:
        AudioPlayer(){};
        AudioPlayer(gpio_num_t _bclk_pin, gpio_num_t _ws_pin, gpio_num_t _dout_pin);
        ~AudioPlayer();
        
        void set_file(WavFile *_wav_file);
        static void play_file(void *parameters);
    protected:
        inline static WavFile *wav_file;
        int sample_rate;
        short bits_per_sample;
        esp_err_t i2s_setup(gpio_num_t _bclk_pin, gpio_num_t _ws_pin, gpio_num_t _dout_pin);
        inline static i2s_chan_handle_t tx_handle;
        inline static SemaphoreHandle_t mutex;
};

#endif