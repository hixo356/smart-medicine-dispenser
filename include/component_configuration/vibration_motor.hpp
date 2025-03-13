#ifndef VIBRATION_MOTOR_HPP
#define VIBRATION_MOTOR_HPP

#include "driver/gpio.h"

class VibrationMotor {
    public:
        VibrationMotor(gpio_num_t _pin);

        void vibrate(int ms_to_vibrate);
    protected:
        gpio_num_t pin;
};

#endif