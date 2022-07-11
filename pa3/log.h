#ifndef PA2_LOG_H
#define PA2_LOG_H

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pa2345.h"
#include "common.h"
#include "banking.h"

typedef enum {
    LOG_STARTED,
    LOG_ALL_STARTED,
    LOG_DONE,
    LOG_ALL_DONE,
    LOG_TRANSFER_IN,
    LOG_TRANSFER_OUT,
    LOG_STOP,
    LOG_ACK,
    LOG_LOOP,
    LOG_COPIED,
} LogFormat;

typedef enum {
    PIPE_FAIL,
    PIPE_FAIL_NONBLOCK,
    PIPE_STARTED,
    PIPE_SEND,
    PIPE_RECEIVE,
    PIPE_READ_AGAIN,
    PIPE_ERROR,
} PipeFormat;

extern FILE * PipesLog;
extern FILE * EventsLog;

void openLog();
void closeLog();

void logEvent(LogFormat format, local_id src, local_id dst, balance_t balance, timestamp_t time);
void logPipe(PipeFormat format, local_id src, local_id dst, int size, timestamp_t time);

#endif //PA2_LOG_H
