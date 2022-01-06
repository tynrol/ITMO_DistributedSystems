#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <sys/io.h>
#include <string.h>
#include <fcntl.h>
#include "log.h"


#define MAX_PROCESS_COUNT 11

typedef struct{
    int fdRead;
    int fdWrite;
} PipeChannel;

typedef struct{
    int process_count;
    local_id current_id;
    PipeChannel *pipes[MAX_PROCESS_COUNT][MAX_PROCESS_COUNT];
} Mesh;

Message createMessage(uint16_t magic, local_id id, MessageType type, timestamp_t time);
MessageHeader createMessageHeader(uint16_t magic, uint16_t len, int16_t type, timestamp_t time);
