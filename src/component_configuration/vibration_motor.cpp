#include "component_configuration/vibration_motor.hpp"

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

VibrationMotor::VibrationMotor(gpio_num_t _pin){
    pin = _pin;

    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
}

void VibrationMotor::vibrate(int ms_to_vibrate){

    gpio_set_level(pin, 1);
    vTaskDelay(pdMS_TO_TICKS(ms_to_vibrate));
    gpio_set_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(10));

}