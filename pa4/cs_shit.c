#include "cs_shit.h"

int request_cs(const void *self) {
    Message request = createMessage(MESSAGE_MAGIC, 0,0,0, CS_REQUEST);
    Info new_info;
    new_info.time = time_clock;
    new_info.id = id;
    push(new_info);
    send_multicast(&id, &request);
    while (entered_mutex() != 1) {
        Message received;
        local_id from = get(&received);
        if (from > 0) {
            switch (received.s_header.s_type) {
                case CS_REQUEST: {
                    Message response = createMessage(MESSAGE_MAGIC, 0, 0, 0, CS_REPLY);
                    Info new_info;
                    new_info.id = from;
                    new_info.time = received.s_header.s_local_time;
                    push(new_info);
                    fprintf(EventsLog, "%d: process %i CS_REQUEST from %i\n",time_clock, id, from);
                    fflush(EventsLog);
                    send(&id, from, &response);
                    break;
                }
                case CS_REPLY: {
                    set_reply(from);
                    fprintf(EventsLog, "process %i CS_REPLY from %i\n", id, from);
                    fflush(EventsLog);
                    break;
                }
                case CS_RELEASE: {
                    if (queue.info[0].id == from) {
                        pop();
                    }
                    fprintf(EventsLog, "process %i CS_RELEASE from %i\n", id, from);
                    fflush(EventsLog);
                    break;
                }
                case DONE: {
                    set_done(from);
                    fprintf(EventsLog, "process %i DONE from %i", id, from);
                    fflush(EventsLog);
                }
                default: {
                    break;
                }
            }
        }
    }
    return 0;
}

int release_cs(const void *self) {
    pop();
    Message release = createMessage(MESSAGE_MAGIC, 0,0,0,CS_RELEASE);
    for (int i = 1; i <= process_count; i++) {
        queue.replies[i] = id != i ? 0 : 1;
    }
    send_multicast(&id, &release);
    return 0;
}

void set_done(int i) {
    queue.done[i] = 1;
}

int exit_mutex() {
    for (int i = 1; i <= process_count; i++){
        if (queue.done[i] == 0)
            return 0;
    }
    return 1;
}

void set_reply(int i) {
    queue.replies[i] = 1;
}

int entered_mutex() {
    if (queue.info[0].id == id) {
        for(int i = 1; i<=process_count; i++) {
            if (queue.replies[i] == 0){
                return 0;
            }
        }
    } else {
        return 0;
    }
    return 1;
}


