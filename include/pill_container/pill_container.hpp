#ifndef PILL_CONTAINER_HPP
#define PILL_CONTAINER_HPP

#include "component_configuration/servo.h"
#include "component_configuration/vibration_motor.hpp"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"

class PillContainer{
    public:
        PillContainer(){};
        PillContainer(Servo &_servo, VibrationMotor &_vib_motor, int _id);

        int get_pill_quantity() { return pill_quantity; }
        SemaphoreHandle_t &get_mutex() { return mutex; }
        void cont_start_dispensing(int count);
        int get_id(){ return id; }
        Servo *get_servo(){ return servo; }
    protected:
        int id;
        int pill_quantity;
        SemaphoreHandle_t mutex;
        Servo *servo;
        VibrationMotor *vib_motor;

        void dispense(int count);
        static void start_dispensing_task(void* _params);
        //some bool to make sure everything in device is closed and secure? limit switch maybe
};

#endif