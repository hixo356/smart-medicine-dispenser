#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <esp_wifi.h>
#include <esp_event.h>
#include <freertos/event_groups.h>
#include <esp_netif.h>

#define WIFI_SSID "cybulskiego 5G"
#define WIFI_PASS "eok7khz4y92f8exz" //eok7khz4y92f8exz

// #define WIFI_SSID "NETIASPOT-B96130"
// #define WIFI_PASS "ht8whhfx84u3"

class wifi_connection
{
private:
    EventGroupHandle_t wifi_event_group;
    const int CONNECTED_BIT = BIT0;
public:
    wifi_connection();
    ~wifi_connection();
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};

#endif

