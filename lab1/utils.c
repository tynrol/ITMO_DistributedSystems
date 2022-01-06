#include "utils.h"

Message createMessage(uint16_t magic, local_id id, MessageType type, timestamp_t time){
    Message message;
    MessageHeader messageHeader;
    uint16_t len;
    switch (type) {
        case DONE:
            len = logEvent(EVENT_DONE, id);
            break;
        case STARTED:
            len = logEvent(EVENT_STARTED, id);
            break;
        default:
            len = 0;
            break;
    }
    messageHeader = createMessageHeader(magic, len, type, time);
    message.s_header = messageHeader;
    return message;
}

MessageHeader createMessageHeader(uint16_t magic, uint16_t len, int16_t type, timestamp_t time){
    MessageHeader header;
    header.s_magic = MESSAGE_MAGIC;
    header.s_payload_len = len;
    header.s_type = type;
    header.s_local_time = time;
    return header;
}
