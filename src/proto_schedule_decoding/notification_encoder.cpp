#include "proto_decoding/notification_encoder.hpp"
#include <cstdio>

bool NotificationEncoder::encodeNotification(NotificationType notification_id, uint8_t* buffer, size_t buffer_size, size_t* message_length) {
    // Initialize the notification message
    medication_notification notification = medication_notification_init_default;
    notification.notification_id = notification_id;

    // Create a stream that writes to the buffer
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);

    // Serialize the message
    bool status = pb_encode(&stream, medication_notification_fields, &notification);

    if (!status) {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return false;
    }

    // Set the message length
    *message_length = stream.bytes_written;
    return true;
}
