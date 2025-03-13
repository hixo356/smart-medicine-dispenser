#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include "proto/info_messages.pb.h"
#include "proto_decoding/schedule_decoder.hpp"
#include "schedule/schedule.hpp"

extern "C"
{
    bool time_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
        printf("Time decode callback\n");
        ProtoDecoder::container_time_data_t *time_data = (ProtoDecoder::container_time_data_t*)(*arg);

        if (time_data->time_count >= MAX_TIME_COUNT) {
            printf("Too many time elements\n");
            return false; // Za dużo elementów
        }

        int64_t value;
        if (!pb_decode_varint(stream, (uint64_t*)&value)) {
            printf("Failed to decode time value\n");
            return false;
        }

        time_data->time_values[time_data->time_count++] = value;
        printf("Decoded time value: %lld\n", (long long)value); // Log dla zdekodowanej wartości
        return true;
    }

    bool container_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {
        printf("Container decode callback\n");
        ProtoDecoder::schedule_containers_data_t *containers_data = (ProtoDecoder::schedule_containers_data_t*)(*arg);

        if (containers_data->container_count >= MAX_CONTAINERS_COUNT) {
            printf("Too many elements\n");
            return false; // Too many elements 
        }

        ProtoDecoder::container_with_data_t *new_container = &containers_data->containers[containers_data->container_count];

        new_container->base.time.funcs.decode = &time_decode_callback;
        new_container->base.time.arg = &new_container->time_data;


        if(!pb_decode(stream, container_fields, &containers_data->containers[containers_data->container_count].base)) {
            printf("Failed to decode container\n");
            return false;
        }

        containers_data->container_count++;
        return true;
    }

    void init_container_with_data(ProtoDecoder::container_with_data_t *container) {
        container->base.time.funcs.decode = &time_decode_callback;
        container->base.time.arg = &container->time_data;
    }

    void init_schedule_with_data(ProtoDecoder::schedule_with_data_t *schedule) {
        schedule->base.containers.funcs.decode = &container_decode_callback;
        schedule->base.containers.arg = &schedule->containers_data;
    }

    bool ProtoDecoder::get_schedule(const pb_byte_t *buffer, size_t buffer_len, daily_schedule_t &schedule)
    {
        init_schedule_with_data(&decoded_schedule);
        pb_istream_t stream = pb_istream_from_buffer(buffer, buffer_len);
        status = pb_decode(&stream, schedule_fields, &decoded_schedule.base);

        if (!status) {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        } else {
            printf("\n\n\nDecoding successful! Containers count: %d\n", decoded_schedule.containers_data.container_count);
            daily_schedule_t schedule_temp;

            for (size_t i = 0; i < decoded_schedule.containers_data.container_count; i++) {
                container_with_data_t *decoded_container = &decoded_schedule.containers_data.containers[i];
                printf("Container ID: %ld\n", decoded_container->base.ID);
                printf("Dose: %ld\n", decoded_container->base.dose);
                schedule_temp.containers[i].dose = decoded_container->base.dose;
                printf("Quantity: %ld\n", decoded_container->base.quantity);
                printf("Times count: %d\n", decoded_container->time_data.time_count);
                schedule_temp.containers[i].times_count = decoded_container->time_data.time_count;
                for (size_t j = 0; j < decoded_container->time_data.time_count; j++) {
                    printf("Time: %lld\n", (long long)decoded_container->time_data.time_values[j]);
                    schedule_temp.containers[i].times[j] = decoded_container->time_data.time_values[j];
                }
                printf("\n\n\n");
            }

            schedule = schedule_temp;
        }
        //printf("\n\n%d\n\n", schedule_temp)
        return status;
    }
}