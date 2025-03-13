#include "dispensation_handler/dispensation_handler.hpp"
#include "proto_decoding/notification_encoder.hpp"
#include "esp_log.h"
#include "esp_netif_sntp.h"

#include <string>
#include <sys/time.h>
#include <queue>
#include <vector>

#define CONTAINER_COUNT 2
#define TAG "dispensation-handler"

DispensationHandler::DispensationHandler(Device &_device){
    device = &_device;
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);
    // setenv("TZ", "CST-2", 1);
    tzset();
}

// main function handling entire dispensation process
// called once it's the time, for the specified time slot
void DispensationHandler::start_dispensing(int *dose_from_each_container){

    ESP_LOGI(TAG, "start_dispensing task running");

    printf("\n\nSTART_DISPENSING\n");
    for(int i=0; i<5; i++){
        printf("%d: %d\n", i, dose_from_each_container[i]);
    }

    for(int cont_idx = 0; cont_idx < 5; cont_idx++){
        if(dose_from_each_container[cont_idx] == 0) continue;
        device->get_container(cont_idx).cont_start_dispensing(dose_from_each_container[cont_idx]);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // wait until all dispensing is done
    ESP_LOGI(TAG, "waiting for dispensation to be done...");
    while(!device->dispensing_done()){ vTaskDelay(50); };

    ESP_LOGI(TAG, "playing sound notification");
    // device->play_sound("/data/wydano_leki2.wav");

    uint8_t buffer[medication_notification_size];

    size_t message_length = 0;

    // Encode notification
    if (NotificationEncoder::encodeNotification(NotificationType_TAKE_MEDICATION, buffer, sizeof(buffer), &message_length)) {
        printf("Message encoded successfully, size: %d bytes\n", message_length);
        // Send the encoded message to the server
        // ServerConnection server;
        server.post("http://192.168.0.100:8000/receive_medication_notification/", buffer, message_length);
    } else {
        printf("Failed to encode message\n");
    }


    time_t now;
    time(&now);
    time_t ten_minutes = now + 60; //current time + 10 minutes
    while(now != ten_minutes){ //wait ten minutes
        time(&now);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // check whether pill tray returned to its place
    if(!device->pill_tray_in_place()){
        if (NotificationEncoder::encodeNotification(NotificationType_NO_CUP_UNDER_DISPENSER, buffer, sizeof(buffer), &message_length)) {
            printf("Message encoded successfully, size: %d bytes\n", message_length);
            // Send the encoded message to the server
            // ServerConnection server;
            server.post("http://192.168.0.100:8000/receive_medication_notification/", buffer, message_length);
        } else {
            printf("Failed to encode message\n");
        }

        // device->play_sound(); //powtorzenie_10.wav

        time(&now);
        time_t five_minutes = now + 300;
        while(now != five_minutes){ //wait five minutes
            time(&now);
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        if(!device->pill_tray_in_place()){
            // post to backend, tray still not returned

            // device->play_sound(); //powtorzenie_15.wav
        } else {
            // post to backend, taken with significant delay
        }
    } else {
            if (NotificationEncoder::encodeNotification(NotificationType_MEDICATION_TAKEN, buffer, sizeof(buffer), &message_length)) {
                printf("Message encoded successfully, size: %d bytes\n", message_length);
                // Send the encoded message to the server
                // ServerConnection server;
                server.post("http://192.168.0.100:8000/receive_medication_notification/", buffer, message_length);
            } else {
                printf("Failed to encode message\n");
            }
    }

    // pill quantity in containers stuff?

}

typedef struct {
    void *handler;
    int *dose_from_each_container;
} params_t;

void DispensationHandler::start_dispensing_task(void *parameters){
    params_t *params = (params_t*)parameters;
    ((DispensationHandler*)params->handler)->start_dispensing(params->dose_from_each_container);
    vTaskDelete(NULL);
}

void DispensationHandler::get_schedule_from_server(){
    // std::string url = (std::string)device->get_config().backend_data.ip_addr + ":" + (std::string)device->get_config().backend_data.port + "/schedules";
    std::string url = "192.168.0.100:8000/schedules";
    ESP_LOGI(TAG, "Sending GET request for new medication schedule...");
    if(server.get("http://192.168.0.100:8000/send_medication_data/") == ESP_OK){  //url.c_str()
        ESP_LOGI(TAG, "Response received.");
        const pb_byte_t *buffer = server.get_buffer();
        size_t buffer_len = server.get_buffer_len();
        daily_schedule_t decoded_schedule = {0};

        decoder.get_schedule(buffer, buffer_len, decoded_schedule);

        printf("\n%d\n", decoded_schedule.containers[0].dose);
        set_schedule(decoded_schedule);
        printf("\n%d\n", get_schedule().containers[0].dose);
        ESP_LOGI(TAG, "Schedule set.");
    } else {
        ESP_LOGW(TAG, "Request failed, taking schedule from backup file.");
        // set_schedule(); ustawienie schedule z pliku
    }
}

typedef struct {
    int time;
    int dose;
    int container_idx;
} single_dose_t;

class QueueCompare{
    public:
        bool operator()(single_dose_t &lhs, single_dose_t &rhs){
            if (lhs.time < rhs.time) return true;
            else return false; 
        }
};

void DispensationHandler::loop(){
    get_schedule_from_server();
    
    time_t now;
    
    time(&now);
    printf("\n\nTIME: %s\n\n", ctime(&now));

    esp_netif_sntp_start();

    int dose_from_each_container[5] = {0};
    std::priority_queue<single_dose_t, std::vector<single_dose_t>, QueueCompare> dosages;

    daily_schedule_t todays_schedule = get_schedule();
    printf("\n\nSCHEDULE\n");
    for (int i = 0; i < 5; i++)
    {
        printf("%d\n", todays_schedule.containers[i].dose);
    }
    
    single_dose_t dosage = {0};

    printf("\n\nDOSAGES\n\n");
    for(int i=0; i<5; i++){
        // printf("\n\n%d\n\n", todays_schedule.containers[i].times_count);
        printf("\n\n%d:\n", i);
        for(int j=0; j<todays_schedule.containers[i].times_count; j++){
            dosage.time = todays_schedule.containers[i].times[j];
            // printf("%d\n", dosage.time);
            dosage.dose = todays_schedule.containers[i].dose;
            dosage.container_idx = i;
            printf("\ndose:\n%d\ntime:\n%d", dosage.dose, dosage.time);

            dosages.push(dosage);
        }
    }

    printf("\nSIZE: %d\n", dosages.size());

    while(1){
        time(&now);
        
        if(dosages.top().time <= now && dosages.size() != 0){
            while(dosages.top().time <= now && dosages.size() != 0){
                dose_from_each_container[dosages.top().container_idx] = dosages.top().dose;
                dosages.pop();
            }
            printf("\n\nDOSE_FROM_EACH:\n");
            for(int i=0; i<5; i++){
                printf("%d\n", dose_from_each_container[i]);
            }
            params_t params = {this, dose_from_each_container};
            xTaskCreate(start_dispensing_task, "Handler dispensing task", 8192, (void *)&params, 5, NULL);
            vTaskDelay(pdMS_TO_TICKS(100000));

            for(int i=0; i<5; i++){
                dose_from_each_container[i] = 0;
            }
        }
        

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void DispensationHandler::begin_task(void* parameters){
    ((DispensationHandler*)parameters)->loop();
    vTaskDelete(NULL);
}

void DispensationHandler::begin(){
    xTaskCreate(begin_task, "Dispensation Handler", 8192, this, 5, NULL);
}