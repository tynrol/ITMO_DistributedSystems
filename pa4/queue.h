#ifndef PA4_QUEUE_H
#define PA4_QUEUE_H

#include "pipes.h"

//typedef enum {
//    NONE = 0,
//    RECEIVED
//} r_status;

typedef struct {
    local_id id;
    timestamp_t time;
} Info;

typedef struct {
    int queue_len;
    Info info[11];
    int replies[11];
    int done[11];
} Queue;

extern Queue queue;

void initQueue();
Info pop();
void push(Info info);
int compare(Info left, Info right);

#endif //PA4_QUEUE_H
