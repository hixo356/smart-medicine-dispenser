#ifndef SERVO_H
#define SERVO_H

#include <driver/gpio.h>
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

class Servo
{
private:
    float pulse_width_0_degrees = 0.55; // Pulse width for 0 degrees, in milliseconds
    float pulse_width_180_degrees = 2.5; // Pulse width for 180 degrees, in milliseconds

    float period = 1000.0 / 50; // Period of the PWM signal, in milliseconds

    float duty_cycle_0_degrees = (pulse_width_0_degrees / period) * 100;
    float duty_cycle_180_degrees = (pulse_width_180_degrees / period) * 100;
    mcpwm_unit_t _unit;
    mcpwm_timer_t _timer;
public:
    Servo(mcpwm_unit_t unit, mcpwm_timer_t timer,mcpwm_io_signals_t io, gpio_num_t SERVO_PIN);
    void servo_dispense();

    float getDutyCycle0Degrees() { return duty_cycle_0_degrees; }
    float getDutyCycle180Degrees() { return duty_cycle_180_degrees; }
};

#endif