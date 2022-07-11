#ifndef PA3_IPC_TIME_H
#define PA3_IPC_TIME_H

#include <stdio.h>
#include <string.h>
#include "log.h"
#include "ipc.h"
#include "pipes.h"

extern timestamp_t time_clock;
void update_time(timestamp_t time);

local_id get(Message *received);
int receive_with_time(void * self, local_id from, Message * msg);

void transaction_in(timestamp_t time, balance_t amount, local_id from);
void transaction_out(timestamp_t time, balance_t amount, local_id to);

Message createMessage(uint16_t magic, local_id src, local_id dst, balance_t balance, MessageType type);
MessageHeader createMessageHeader(uint16_t magic, uint16_t len, int16_t type, timestamp_t time);

#endif //PA3_IPC_TIME_H
