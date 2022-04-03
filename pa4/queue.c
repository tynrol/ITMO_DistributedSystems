#include "queue.h"

Queue queue;

void initQueue() {
    queue.queue_len = 0;
    queue.info->id = 0;
    queue.info->time = time_clock;
    for (int i = 0; i<11;i++) {
        queue.replies[i] = i != id ? 0 : 1;
        queue.done[i] = i != id ? 0 : 1;
    }
}

Info pop() {
    if (queue.queue_len > 0) {
        if (queue.queue_len == 1) {
            queue.queue_len--;
            return queue.info[0];
        } else {
            Info value = queue.info[0];
            for (int i = 1; i < queue.queue_len; i++) {
                queue.info[i - 1] = queue.info[i];
            }
            queue.queue_len--;
            return value;
        }
    }
    Info info;
    info.time = -1;
    info.id = -1;
    return info;
}

int compare(Info left, Info right) {
    if (left.time != right.time) {
        if (left.time > right.time)
            return 1;
        else if (left.time == right.time)
            return 0;
        else
            return -1;
    } else {
        if (left.id > right.id)
            return 1;
        else if (left.id == right.id)
            return 0;
        else
            return -1;
    }
}

void push(Info new_info) {
    if (queue.queue_len == 0) {
        queue.queue_len++;
        queue.info[0] = new_info;
        return;
    }
    int insert_pos = queue.queue_len;
    for (int i = 0; i < queue.queue_len; i++) {
        if (compare(new_info, queue.info[i]) < 1) {
            insert_pos = i;
        }
    }
    for (int i = queue.queue_len - 1; i >= insert_pos; i--) {
        queue.info[i + 1] = queue.info[i];
    }
    queue.info[insert_pos] = new_info;
    queue.queue_len++;
}
