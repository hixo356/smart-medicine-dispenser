#ifndef NOTIFICATION_ENCODER_HPP
#define NOTIFICATION_ENCODER_HPP

#include <pb_encode.h>
#include "proto/info_messages.pb.h"

class NotificationEncoder {
public:
    static bool encodeNotification(NotificationType notification_id, uint8_t *buffer, size_t buffer_size, size_t* message_length);
};

#endif // NOTIFICATION_ENCODER_HPP
