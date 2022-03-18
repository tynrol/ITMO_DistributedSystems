#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/io.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "log.h"

#define MAX_PROCESS_COUNT 11

typedef struct{
    int fdRead;
    int fdWrite;
} PipeChannel;

typedef struct{
    int process_count;
    local_id current_id;
    BalanceState *balanceStates[MAX_PROCESS_COUNT];
    PipeChannel *pipes[MAX_PROCESS_COUNT][MAX_PROCESS_COUNT];
} Mesh;

Message createMessage(uint16_t magic, local_id src, local_id dst, balance_t balance, MessageType type, timestamp_t time);
MessageHeader createMessageHeader(uint16_t magic, uint16_t len, int16_t type, timestamp_t time);
TransferOrder createTransferOrder(local_id src, local_id dst, balance_t balance);
