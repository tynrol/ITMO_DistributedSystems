#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pa2345.h"
#include "common.h"
#include "banking.h"
#include "ipc.h"

typedef enum {
    EVENT_STARTED,
    EVENT_RECV_ALL_STARTED,
    EVENT_DONE,
    EVENT_RECV_ALL_DONE,
    EVENT_TRANSFER_IN,
    EVENT_TRANSFER_OUT,
    EVENT_STOP,
} EventStatus;

typedef enum {
    OPEN,
    CLOSED
} PipeStatus;

void openLogEvent();
void openLogPipe();

void closeLogEvent();
void closeLogPipe();

char* logEvent(EventStatus status, local_id id, balance_t balance, timestamp_t timestamp, local_id foreign_id);
//void logPipe(PipeStatus status, local_id id, int from, int to, int descriptor);
