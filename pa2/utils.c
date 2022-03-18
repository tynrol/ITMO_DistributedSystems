#include "utils.h"


Message createMessage(uint16_t magic, local_id src, local_id dst, balance_t balance, MessageType type, timestamp_t time){
    Message message;
    MessageHeader messageHeader;
    char *buf;

    switch (type) {
        case DONE:
            buf = logEvent(EVENT_DONE, src, balance, time, 0);
            messageHeader = createMessageHeader(magic, strlen(buf), type, time);
            message.s_header = messageHeader;
            strcpy(message.s_payload, buf);
            return message;
        case STARTED:
            buf = logEvent(EVENT_STARTED, src, balance, time, 0);
            messageHeader = createMessageHeader(magic, strlen(buf), type, time);
            message.s_header = messageHeader;
            strcpy(message.s_payload, buf);
            return message;
        case TRANSFER:
            logEvent(EVENT_TRANSFER_OUT, src, balance, time, dst);
            messageHeader = createMessageHeader(magic, sizeof(TransferOrder), type, time);
            message.s_header = messageHeader;
            TransferOrder order = createTransferOrder(src, dst, balance);
            memcpy(message.s_payload, &order, sizeof(TransferOrder));
            return message;
        case STOP:
            logEvent(EVENT_STOP, src, balance, time, 0);
            messageHeader = createMessageHeader(magic, 0, type, time);
            message.s_header = messageHeader;
            // strcpy(message.s_payload, "");
            return message;     
        case ACK: 
            messageHeader = createMessageHeader(magic, 0, type, time);
            message.s_header = messageHeader;
            return message;  
        default:
            break;
    }
}

MessageHeader createMessageHeader(uint16_t magic, uint16_t len, int16_t type, timestamp_t time){
    MessageHeader header;
    header.s_magic = MESSAGE_MAGIC;
    header.s_payload_len = len;
    header.s_type = type;
    header.s_local_time = time;
    return header;
}

TransferOrder createTransferOrder(local_id src, local_id dst, balance_t balance){
    TransferOrder order;
    order.s_src = src;
    order.s_dst = dst;
    order.s_amount = balance;
    return order;
}


