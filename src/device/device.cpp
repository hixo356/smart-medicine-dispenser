#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "esp_log.h"
#include "esp_err.h"

#include "device/device.hpp"
#include "component_configuration/servo.h"
#include "wifi_connection/wifi_connection.h"
#include "component_configuration/vibration_motor.hpp"

#include <exception>

#define TAG "device"

#define PILL_REFILL_LEVEL 4

void Device::device_init(device_cfg_t dev_cfg){

    config = dev_cfg;

    static AudioPlayer new_audio(dev_cfg.audio_pins.bclk, dev_cfg.audio_pins.ws, dev_cfg.audio_pins.dout);
    audio_player = new_audio;

    static Servo servo1(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, dev_cfg.servo_pins.servo1);
    static Servo servo2(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM1A, dev_cfg.servo_pins.servo2);
    static Servo servo3(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM2A, dev_cfg.servo_pins.servo3);
    static Servo servo4(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM0A, dev_cfg.servo_pins.servo4);
    static Servo servo5(MCPWM_UNIT_1, MCPWM_TIMER_1, MCPWM1A, dev_cfg.servo_pins.servo5);

    static VibrationMotor vib_motor1(dev_cfg.vibration_motors_pins.motor1);
    VibrationMotor vib_motor2(dev_cfg.vibration_motors_pins.motor2);
    VibrationMotor vib_motor3(dev_cfg.vibration_motors_pins.motor3);
    VibrationMotor vib_motor4(dev_cfg.vibration_motors_pins.motor4);
    VibrationMotor vib_motor5(dev_cfg.vibration_motors_pins.motor5);

    PillContainer cont1 = PillContainer(servo1, vib_motor1, 1);
    PillContainer cont2 = PillContainer(servo2, vib_motor2, 2);
    PillContainer cont3 = PillContainer(servo3, vib_motor3, 3);
    PillContainer cont4 = PillContainer(servo4, vib_motor4, 3);
    PillContainer cont5 = PillContainer(servo5, vib_motor5, 3);

    containers[0] = cont1;
    containers[1] = cont2;
    containers[2] = cont3;
    containers[3] = cont4;
    containers[4] = cont5;

    Sensor _cup_sensor(dev_cfg.limit_switch_pins.pill_tray_switch);
    cup_sensor = _cup_sensor;

    Sensor _drawer_sensor(dev_cfg.limit_switch_pins.drawer_switch);
    drawer_sensor = _drawer_sensor;

    // wifi_connection _wifi(dev_cfg.wifi_credentials.ssid, dev_cfg.wifi_credentials.password);
    // wifi = _wifi;

    //backend init (with event loop handle passed)
}

bool Device::pill_tray_in_place(){
    return cup_sensor.IsPlaced();
}

bool Device::drawer_closed(){
    return drawer_sensor.IsPlaced();
}

void Device::play_sound(const char *path){
    static WavFile file(path);

    audio_player.set_file(&file);

    xTaskCreate(audio_player.play_file, "play_wav_file", 8192, NULL, 5, NULL);
}

bool Device::dispensing_done(){
    SemaphoreHandle_t mutex_0 = containers[0].get_mutex();
    SemaphoreHandle_t mutex_1 = containers[1].get_mutex();
    SemaphoreHandle_t mutex_2 = containers[2].get_mutex();
    SemaphoreHandle_t mutex_3 = containers[3].get_mutex();
    SemaphoreHandle_t mutex_4 = containers[4].get_mutex();
    
    if(mutex_0 != NULL){// && mutex_1 != NULL && mutex_2 != NULL && mutex_3 != NULL && mutex_4 != NULL){
        if(xSemaphoreTake(mutex_0, portMAX_DELAY) == pdTRUE &&
           xSemaphoreTake(mutex_1, portMAX_DELAY) == pdTRUE &&
           xSemaphoreTake(mutex_2, portMAX_DELAY) == pdTRUE &&
           xSemaphoreTake(mutex_3, portMAX_DELAY) == pdTRUE &&
           xSemaphoreTake(mutex_4, portMAX_DELAY) == pdTRUE)
        {
            xSemaphoreGive(mutex_0);
            xSemaphoreGive(mutex_1);
            xSemaphoreGive(mutex_2);
            xSemaphoreGive(mutex_3);
            xSemaphoreGive(mutex_4);
            return true;
        }
    }

    return false;
}

void Device::pill_quantity_check(){
    if(containers[0].get_pill_quantity() < PILL_REFILL_LEVEL) ESP_LOGW(TAG, "Container 1 needs refilling!");
    if(containers[1].get_pill_quantity() < PILL_REFILL_LEVEL) ESP_LOGW(TAG, "Container 2 needs refilling!");
    if(containers[2].get_pill_quantity() < PILL_REFILL_LEVEL) ESP_LOGW(TAG, "Container 3 needs refilling!");
    if(containers[3].get_pill_quantity() < PILL_REFILL_LEVEL) ESP_LOGW(TAG, "Container 4 needs refilling!");
    if(containers[4].get_pill_quantity() < PILL_REFILL_LEVEL) ESP_LOGW(TAG, "Container 5 needs refilling!");
}