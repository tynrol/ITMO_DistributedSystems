#include "utils.h"

int send(void * self, local_id dst, const Message * msg){
    Mesh *mesh = (Mesh *)self;
    if(write(mesh->pipes[mesh->current_id][dst]->fdWrite, msg, msg->s_header.s_payload_len + sizeof(MessageHeader))<=0){
        return 1;
    }
    return 0;
}

int send_multicast(void * self, const Message * msg) {
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

int receive(void * self, local_id from, Message * msg){
    Mesh *mesh = (Mesh *)self;
    MessageHeader messageHeader;

    if(read(mesh->pipes[mesh->current_id][from]->fdRead, &messageHeader,sizeof(MessageHeader)) == -1){
        return 1;
    } else {
        char buffer[messageHeader.s_payload_len];
        if (read(mesh->pipes[mesh->current_id][from]->fdRead, &buffer, messageHeader.s_payload_len) == -1) {
            return 1;
        }
        msg->s_header=messageHeader;
        memcpy(msg->s_payload, buffer, sizeof(msg));
    }


    return 0;
}
//receive all
int receive_any(void * self, Message * msg){
    Mesh *mesh = (Mesh *)self;
    for(local_id i = 1;i<mesh->process_count;i++){
        if(i != mesh->current_id){
            if(receive(mesh, i, msg) != 0){
                return 1;
            }
        }
    }
    return 0;
}

