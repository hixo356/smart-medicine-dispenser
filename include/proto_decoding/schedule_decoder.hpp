#ifndef PROTO_DEOCDE_HPP
#define PROTO_DEOCDE_HPP

#include "pb.h"
#include "pb_decode.h"
#include "proto/info_messages.pb.h"
#include "schedule/schedule.hpp"

#define MAX_CONTAINERS_COUNT 5
#define MAX_TIME_COUNT 8

class ProtoDecoder
{
    public:

        typedef struct {
            int64_t time_values[MAX_TIME_COUNT];
            size_t time_count;
        } container_time_data_t;
        
        typedef struct {
            container base;
            container_time_data_t time_data;
        } container_with_data_t;


        typedef struct {
            container_with_data_t containers[MAX_CONTAINERS_COUNT];
            size_t container_count;
        } schedule_containers_data_t;
        
        typedef struct {
            schedule base;
            schedule_containers_data_t containers_data;
        } schedule_with_data_t;


        bool get_schedule(const pb_byte_t *buffer, size_t buffer_len, daily_schedule_t &schedule);

        bool get_status(){return status;}
        schedule_with_data_t get_decoded_schedule(){return decoded_schedule;}
        
    private:
        bool status;
        schedule_with_data_t decoded_schedule = {0};
};

#endif