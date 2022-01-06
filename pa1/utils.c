#include "utils.h"

Message createMessage(uint16_t magic, local_id id, MessageType type, timestamp_t time){
    Message message;
    MessageHeader messageHeader;
    char *buf;

    switch (type) {
        case DONE:
            buf = logEvent(EVENT_DONE, id);
            break;
        case STARTED:
            buf = logEvent(EVENT_STARTED, id);
            break;
        default:
            buf = "";
            break;
    }
    messageHeader = createMessageHeader(magic, strlen(buf), type, time);
    message.s_header = messageHeader;
//    printf("create buf %s\n", buf);
    strcpy(message.s_payload, buf);
//    printf("create msg %s\n", buf);
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
