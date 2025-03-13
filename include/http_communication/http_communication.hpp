#ifndef SERVER_CONECTION_HPP
#define SERVER_CONECTION_HPP

#include "esp_http_client.h"
#include "pb.h"

class ServerConnection
{
    public:
        ServerConnection();
        ~ServerConnection();
        esp_err_t get(const char *url);
        void post(const char *url, const uint8_t *data, size_t data_length);
        const pb_byte_t *get_buffer();
        size_t get_buffer_len();
        char *get_response_data();

    private:
        esp_http_client_config_t config_get;
        esp_http_client_config_t config_post;
        esp_http_client_handle_t client;
};

#endif