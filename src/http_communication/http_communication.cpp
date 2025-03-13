#include <cstdio>
#include <pb.h>
#include <pb_decode.h>
#include "http_communication/http_communication.hpp"

extern "C"
{

    const pb_byte_t *response_data_buf = NULL;
    char *response_data = NULL;
    int response_data_buf_len;

    esp_err_t client_event_get_handler(esp_http_client_event_t *evt)
    {
        switch (evt->event_id)
        {
        case HTTP_EVENT_ON_DATA:
            if (evt->data != NULL)
            {
                response_data_buf = (pb_byte_t *)evt->data;
                response_data = (char *)evt->data;
                response_data_buf_len = evt->data_len;
            } else {
                printf("HTTP_EVENT_ON_DATA: data is NULL\n");
            }
            break;
        default:
            break;
        }
        return ESP_OK;
    }


}

char *ServerConnection::get_response_data()
{
    return response_data;
}

const pb_byte_t *ServerConnection::get_buffer()
{
    return response_data_buf;
}

size_t ServerConnection::get_buffer_len()
{
    return response_data_buf_len;
}

ServerConnection::ServerConnection()
{
    config_get = {
        .method = HTTP_METHOD_GET,
        .event_handler = client_event_get_handler,
        .buffer_size = 2048,
    };

    config_post = {
        .method = HTTP_METHOD_POST,
        .buffer_size = 2048,
        .buffer_size_tx = 2048,
    };
}


ServerConnection::~ServerConnection()
{
    if (client){
        esp_http_client_cleanup(client);
    }
}

esp_err_t ServerConnection::get(const char *url)
{
    config_get.url = url;
    printf("GET request to: %s\n", config_get.url);

    client = esp_http_client_init(&config_get);

    printf("Performing GET request...\n");

    return esp_http_client_perform(client);
}

void ServerConnection::post(const char *url, const uint8_t *data, size_t data_length)
{
    config_post.url = url;
    printf("POST request to: %s\n", config_post.url);

    client = esp_http_client_init(&config_post);
    esp_http_client_set_header(client, "Content-Type", "application/octet-stream");
    esp_http_client_set_post_field(client, reinterpret_cast<const char *>(data), data_length);

    printf("Performing POST request...\n");

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        printf("HTTP POST Status = %d, content_length = %lld\n",
               esp_http_client_get_status_code(client),
               esp_http_client_get_content_length(client));
    }
    else
    {
        printf("HTTP POST request failed: %s\n", esp_err_to_name(err));
    }
}
