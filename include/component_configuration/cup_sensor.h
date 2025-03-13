#ifndef CUP_SENSOR_H
#define CUP_SENSOR_H

#include <driver/gpio.h>

class Sensor 
{
private:
    gpio_num_t _sensor_pin;

public:
    Sensor(){};
    Sensor(gpio_num_t sensor_pin);
    bool IsPlaced();
};

#endif