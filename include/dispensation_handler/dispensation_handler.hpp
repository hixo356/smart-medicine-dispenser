#ifndef DISPENSATION_HANDLER_HPP
#define DISPENSATION_HANDLER_HPP

#include "device/device.hpp"
#include "schedule/schedule.hpp"
#include "http_communication/http_communication.hpp"
#include "proto_decoding/schedule_decoder.hpp"

class DispensationHandler{
    public:
        DispensationHandler(Device &_device);
        static void start_dispensing_task(void* parameters);
        void set_schedule(daily_schedule_t _schedule){ schedule.set_schedule(_schedule); };
        daily_schedule_t get_schedule(){ return schedule.get_schedule(); };
        void get_schedule_from_server();
        void begin(); //start handler's loop etc
        ServerConnection server;
        ProtoDecoder decoder;
    protected:
        Device *device;
        Schedule schedule;
        void start_dispensing(int dose_from_each_container[5]);
        void loop();
        static void begin_task(void* parameters);
};

#endif 