#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "pill_container/pill_container.hpp"
#include "audio_player/audio_player.hpp"
#include "component_configuration/cup_sensor.h"
#include "wifi_connection/wifi_connection.h"

#include "driver/gpio.h"

#define SOUND_FIRST_NOTIFY "/data/test.wav"
#define SOUND_SECOND_NOTIFY "/data/test2.wav"
#define SOUND_SUCCESS "/data/test3.wav"
#define SOUND_RETURN_TRAY "/data/test4.wav"

#define CONTAINER_COUNT 5

typedef struct {
    struct {
        gpio_num_t bclk;
        gpio_num_t ws;
        gpio_num_t dout;
    } audio_pins;
    struct {
        gpio_num_t servo1;
        gpio_num_t servo2;
        gpio_num_t servo3;
        gpio_num_t servo4;
        gpio_num_t servo5;
    } servo_pins;
    struct {
        gpio_num_t pill_tray_switch;
        gpio_num_t drawer_switch;
    } limit_switch_pins;
    // struct {
    //     gpio_num_t wifi_diode;
    //     gpio_num_t power_diode;
    //     gpio_num_t dosing_status_diode;
    // } status_diode_pins;
    struct {
        gpio_num_t motor1;
        gpio_num_t motor2;
        gpio_num_t motor3;
        gpio_num_t motor4;
        gpio_num_t motor5;
    } vibration_motors_pins;
    struct {
        const char *ssid;
        const char *password;
    } wifi_credentials;
    struct {
        const char *ip_addr;
        const char *port;
    } backend_data;
} device_cfg_t;

class Device {
    public:
        Device(){};

        static void device_init(device_cfg_t dev_cfg);
        void play_sound(const char *path);
        bool pill_tray_in_place();
        bool drawer_closed();
        void pill_quantity_check();
        bool dispensing_done();

        PillContainer &get_container(int index) { return containers[index]; }
        device_cfg_t &get_config() { return config; }
    protected:
        inline static PillContainer containers[CONTAINER_COUNT];
        inline static AudioPlayer audio_player;
        inline static Sensor cup_sensor;
        inline static Sensor drawer_sensor;
        //wifi_connection wifi;
        inline static device_cfg_t config;
};

#endif