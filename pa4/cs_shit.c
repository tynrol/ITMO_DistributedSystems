#include "cs_shit.h"

// TODO insert right into there
void set_done(int i) {
    queue.done[i] = 1;
}

int is_all_done() {
//    for (int i = 1; i <= process_count; i++){
//        if (queue.done[i] == NONE)
//            return 0;
//    }
//    return 1;
    int is = 1;
    for (int i = 1; i <= queue.queue_len; i++) {
        is &= queue.done[i];
    }
    return is;
}

// TODO insert right into there
void set_reply(int i) {
    queue.replies[i] = 1;
}

int is_all_replied() {
//    for (int i = 1; i <= process_count; i++){
//        if (queue.replies[i] == NONE)
//            return 0;
//    }
//    return 1;
    int is = 1;
    for (int i = 1; i <= queue.queue_len; i++) {
        is &= queue.replies[i];
    }
    printf("reply %d %d %d %d\n", queue.replies[0],queue.replies[1],queue.replies[2],queue.replies[3]);
    if (queue.replies[0] != 0)
        exit(1);
    return is;
}

// TODO fix
int entered_mutex() {
//    if (queue.info[0].id == id) {
//        for(int i = 1; i<=process_count; i++) {
//            if (queue.replies[i] == NONE){
//                return 0;
//            }
//        }
//    } else {
//        return 0;
//    }
//    return 1;
    int is_first = queue.info[0].id == id ? 1 : 0;
    return is_first & is_all_replied();
}

int release_cs(const void *self) {
    pop();
    Message release = createMessage(MESSAGE_MAGIC, 0,0,0,CS_RELEASE);
//    for (int i = 1; i <= process_count; i++){
//        if (id != i)
//            queue.replies[i] = NONE;
//        else
//            queue.replies[i] = RECEIVED;
//    }
    for (int i = 1; i <= queue.queue_len; i++) {
        queue.replies[i] = id != i ? 0 : 1;
    }
    send_multicast(&id, &release);
    return 0;
}

int request_cs(const void *self) {
    Message request = createMessage(MESSAGE_MAGIC, 0,0,0, CS_REQUEST);
    Info new_info;
    new_info.time = time_clock;
    new_info.id = id;
    push(new_info);
    send_multicast(&id, &request);
    // TODO fix for sure
    while (entered_mutex() != 1) {
        Message received;
        int from;
        for (int i = 1; i<=process_count; i++) {
            if (i != id) {
                int value = receive(&id, i, &received);
                if (value == 0) {
                    update_time(received.s_header.s_local_time);
                    from = i;
                }
            }
        }
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
            case CS_REPLY:
                set_reply(from);
                fprintf(EventsLog, "%d: process %i CS_REPLY from %i\n",time_clock, id, from);
                fflush(EventsLog);
                break;
            case CS_RELEASE:
                if (queue.info[0].id == from) {
                    pop();
                }
                fprintf(EventsLog, "%d: process %i CS_RELEASE from %i\n",time_clock, id, from);
                fflush(EventsLog);
                break;
//            case DONE:
//                set_done(from);
//                fprintf(EventsLog, "%d: process %i DONE from %i",time_clock, id, from);
//                fflush(EventsLog);
            default:
                fprintf(EventsLog, "%d: process %i type=%hd from %i\n",time_clock, id, received.s_header.s_type, from);
                fflush(EventsLog);
                break;
        }
        switch (received.s_header.s_type) {
            case DONE:
                set_done(from);
                fprintf(EventsLog, "%d: process %i DONE from %i",time_clock, id, from);
                fflush(EventsLog);
        }
    }
    return 0;
}
