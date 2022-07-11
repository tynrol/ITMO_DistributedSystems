#include "ipc.h"
#include "pipes.h"

timestamp_t time_clock = 0;

timestamp_t get_lamport_time() {
    return ++time_clock;
}

void update_time(timestamp_t time) {
    if (time_clock < time)
        time_clock = time;
    get_lamport_time();
}

int receive_with_time(void * self, local_id from, Message * msg) {
    int i = receive(self, from, msg);
    if(i == 0) {
        update_time(msg->s_header.s_local_time);
        get_lamport_time();
    }
    return i;
}

local_id get(Message *received) {
    for (local_id i = 1; i <= process_count; i++) {
        if (i != id && receive_with_time(&id, i, received) == 0) {
            return i;
        }
    }
    return -1;
}

//void transaction_in(timestamp_t time, balance_t amount, local_id from) {
//    current += amount;
//    addToHistory(time, current, 0);
//    fprintf(EventsLog, log_transfer_in_fmt, time, id, amount, from);
//}
//
//void transaction_out(timestamp_t time, balance_t amount, local_id to) {
//    current -= amount;
//    addToHistory(time, current, 0);
//    fprintf(EventsLog, log_transfer_in_fmt, time, to, amount, id);
//}

Message createMessage(uint16_t magic, local_id src, local_id dst, balance_t balance, MessageType type){
    Message message;
    MessageHeader messageHeader;
    char *buf = malloc(sizeof(char) * 255);
    timestamp_t time = get_lamport_time();
    switch (type) {
        case DONE:
            sprintf(buf, log_done_fmt, time, src, balance);
            messageHeader = createMessageHeader(magic, strlen(buf), type, time);
            message.s_header = messageHeader;
            strcpy(message.s_payload, buf);
            return message;
        case STARTED:
            sprintf(buf, log_started_fmt, time, src, getpid(), getppid(), balance);
            messageHeader = createMessageHeader(magic, strlen(buf), type, time);
            message.s_header = messageHeader;
            strcpy(message.s_payload, buf);
            return message;
//        case TRANSFER:
//            messageHeader = createMessageHeader(magic, sizeof(TransferOrder), type, time);
//            message.s_header = messageHeader;
//            TransferOrder order = createTransferOrder(src, dst, balance);
//            memcpy(message.s_payload, &order, sizeof(TransferOrder));
//            return message;
        case BALANCE_HISTORY:
            //no need to give more params
            messageHeader = createMessageHeader(magic, 0, type, time);
            message.s_header = messageHeader;
            return message;
        default:
            messageHeader = createMessageHeader(magic, 0, type, time);
            message.s_header = messageHeader;
            return message;
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
