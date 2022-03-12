#include "utils.h"

int send(void *self, local_id dst, const Message *msg) {
    Mesh *mesh = (Mesh *) self;
    if (write(mesh->pipes[mesh->current_id][dst]->fdWrite, msg, msg->s_header.s_payload_len + sizeof(MessageHeader)) <=
        0) {
        return 1;
    }
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    Mesh *mesh = (Mesh *) self;
    for (local_id i = 0; i < mesh->process_count; i++) {
        if (i != mesh->current_id) {
            if (send(mesh, i, msg) != 0) {
                return 1;
            }
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    Mesh *mesh = (Mesh *) self;
    MessageHeader messageHeader;
    char buffer[4096];
    switch (read(mesh->pipes[mesh->current_id][from]->fdRead, &messageHeader, sizeof(MessageHeader))) {
        case -1:
            if (errno == EAGAIN) {
                sleep(1);
                return (-1);
            } else {
                perror("read");
                return (-2);
            }
        case 0:
            close(mesh->pipes[mesh->current_id][from]->fdRead);
            return (-3);
        default:
            if (messageHeader.s_payload_len > 0) {
                switch (read(mesh->pipes[mesh->current_id][from]->fdRead, &buffer, messageHeader.s_payload_len)) {
                case -1:
                    if (errno == EAGAIN) {
                        sleep(1);
                        break;
                    } else {
                        perror("read");
                        return (-4);
                    }
                case 0:
                    close(mesh->pipes[mesh->current_id][from]->fdRead);
                    return (-5);
                default:
                    msg->s_header = messageHeader;
                    //TODO: MEMCPY TO TRANSFER ORDER
                    strcpy(msg->s_payload, buffer);
                    return 0;
                }
            } else {
                msg->s_header = messageHeader;
                return 0;
            } 
    }
    return 0;

}

int receive_any(void *self, Message *msg) {
    Mesh *mesh = (Mesh *) self;
    while (1) {
        for (local_id i = 0; i < mesh->process_count; i++) {
            if (i != mesh->current_id) {
                int return_value = receive(mesh, i, msg);
                if (return_value != 0) {
                    return return_value;
                }
            }
        }
    }
}

