#include "pill_container/pill_container.hpp"
#include "esp_log.h"

#define TAG "pill-container"

struct task_params {
    PillContainer *_this;
    int count;
};

PillContainer::PillContainer(Servo &_servo, VibrationMotor &_vib_motor, int _id){
    servo = &_servo;
    vib_motor = &_vib_motor;
    pill_quantity = 0;
    id = _id;
    mutex = xSemaphoreCreateMutex();
    printf("\nMutex address constructor: %p\n", &mutex);
}

//pill count as parameter
void PillContainer::dispense(int count){
    // int count = (int)parameters;
    // printf("\n\nXDXDXDXD11111\n\n");
    ESP_LOGI(TAG, "dispensing %d times from  container, with address ", count); //%p , this, id%d
    // printf("\n\nXDXDXDXD22222\n\n");
    for(int i=0; i<count; i++){
        if(mutex != NULL){
            // printf("\n\nXD\n\n");
            if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
                ESP_LOGI(TAG, "mutex taken, calling servo to dispsense");
                ESP_LOGI(TAG, "called servo: %p", servo);
                this->vib_motor->vibrate(800);
                vTaskDelay(pdMS_TO_TICKS(100));
                this->servo->servo_dispense();
                vTaskDelay(pdMS_TO_TICKS(100));
                xSemaphoreGive(mutex);
            }
        }
    }
}

void PillContainer::start_dispensing_task(void* _params){
    struct task_params *params = (task_params*)_params;
    int count = params->count;
    PillContainer *container = params->_this;
    printf("\n\n%d\n\n%p\n\n", count, container);
    // printf("\nConverted address: %p\n", (PillContainer*)params->_this);
    container->dispense(count); //((PillContainer*)params->_this)
    vTaskDelete(NULL);
}

void PillContainer::cont_start_dispensing(int count){
    struct task_params params = {this, count};
    printf("\nStruct: count - %d, this - %p\n", params.count, params._this);
    xTaskCreate(this->start_dispensing_task, "Dispensing", 8192, (void*)&params, 5, NULL);
    vTaskDelay(pdMS_TO_TICKS(500));
}

