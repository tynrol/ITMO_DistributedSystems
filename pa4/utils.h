#ifndef PA2_UTILS_H
#define PA2_UTILS_H

#include <stdio.h>
#include <string.h>
#include "log.h"
#include "ipc.h"
#include "pipes.h"
#include "history.h"
#include "banking.h"

extern timestamp_t time_clock;

void update_time(timestamp_t time);
void transaction_in(timestamp_t time, balance_t amount, local_id from);
void transaction_out(timestamp_t time, balance_t amount, local_id to);
int receive_all(Message *buffer, local_id from, local_id to);

Message createMessage(uint16_t magic, local_id src, local_id dst, balance_t balance, MessageType type);
MessageHeader createMessageHeader(uint16_t magic, uint16_t len, int16_t type, timestamp_t time);
TransferOrder createTransferOrder(local_id src, local_id dst, balance_t balance);
BalanceState createBalanceState(balance_t s_balance, timestamp_t s_time, balance_t s_balance_pending_in);

#endif //PA2_UTILS_H
