#ifndef PA4_QUEUE_H
#define PA4_QUEUE_H

#include "pipes.h"
#include "utils.h"

typedef struct {
    local_id id;
    timestamp_t time;
} Info;

typedef struct {
    int len;
    Info info[11];
    int replies[11];
    int done[11];
} Queue;

extern Queue queue;

void initQueue();
Info pop();
int compare(Info left, Info right);
void push(Info new_info);

#endif //PA4_QUEUE_H
