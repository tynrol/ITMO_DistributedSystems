#include <malloc.h>
#include <errno.h>
#include "ipc.h"
#include "handler.h"
#include "pipes.h"

int send(void *self, local_id dst, const Message *msg) {
    local_id src = *(local_id *) self;
    int error = write(pipes[src][dst].fd[1], msg, msg->s_header.s_payload_len + sizeof(MessageHeader));
    logPipe(PIPE_SEND, src, dst, error, msg->s_header.s_local_time);
    if (error == -1 || error < msg->s_header.s_payload_len + sizeof(MessageHeader))
        return -1;
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    local_id src = *(local_id *) self;
    for (local_id i = 0; i <= process_count; i++) {
        if (i != src)
            if (send(self, i, msg) == -1)
                return -1;
    }
    return 0;
}

int receive(void *self, local_id src, Message *msg) {
    local_id dst = *(local_id *) self;
    int error = read(pipes[src][dst].fd[0], &(msg->s_header), sizeof(MessageHeader));
    if (error == -1) {
        if (errno == EAGAIN) {
            return 1;
        }
    }
    logPipe(PIPE_RECEIVE, src, dst, error, msg->s_header.s_local_time);
    if (error < sizeof(MessageHeader)) {
        return -1;
    }

    size_t size = msg->s_header.s_payload_len;
    while (1) {
        error = read(pipes[src][dst].fd[0], &(msg->s_payload), size);
        if (size == error) {
            break;
        } else if (errno == EAGAIN && error == -1) {
            logPipe(PIPE_READ_AGAIN, dst, 0, error, msg->s_header.s_local_time);
            continue;
        } else {
            logPipe(PIPE_ERROR, dst, 0, error, msg->s_header.s_local_time);
            return -1;
        }
    }
    logPipe(PIPE_RECEIVE, dst, src, error, msg->s_header.s_local_time);

    return 0;
}

int receive_any(void *self, Message *msg) {
    local_id arg = *(local_id *) self;
    for (local_id i = 0; i <= process_count; i++) {
        if (i != arg) {
            int error = receive(self, i, msg);
            if (error == 0) return 0;
            else if (error == 1) continue;
            else {
                return -1;
            }
        }
    }
    return 1;
}
