#include "handler.h"


int is_mutex = 0;
local_id process_count = 0;
local_id id = 0;
local_id id_parent = 0;
pid_t current = 0;
pid_t parent = 0;
balance_t current_amount = 0;
timestamp_t last_state_time = 0;

void init() {
    parent = getpid();
    current = parent;

    openLog();
    openPipes();

    pid_t pid;
    for (int i = 1; i <= process_count; i++) {
        pid = fork();
        if (pid == -1) {
            printf("Error on creating child: %d\n", i);
            exit(-10);
        }

        if (pid == 0) {
            id = (local_id) i;
            current = pid;
            logEvent(LOG_STARTED, id, id_parent, current_amount, time_clock);
            break;
        }
    }
    closePipes();

    initQueue();
    start();
    work();
    end();

    closeLinePipes();

    if (parent == current) {
        int value;
        for (int i = 1; i <= process_count; i++) {
            if (wait(&value) == -1) {
                printf("Error while ending child\n");
                exit(-1);
            }
        }
        printf("Finished successfully\n");
    }
}

void start() {
    Message started = createMessage(MESSAGE_MAGIC, id, 0, current_amount, STARTED);
    if (current != parent) {
        for (local_id i = 1; i <= process_count; i++) {
            if (i != id) {
                send(&id, i, &started);
            }
        }
    }
    Message received;
    for (local_id i = 1; i <= process_count; i++) {
        if (i != id) {
            int error = receive(&id, i, &received);
            if (error == 0 && received.s_header.s_type != STARTED) {
                exit(-1);
            } else if (error == 1) {
                i--;
            }
            if (error == 0 && received.s_header.s_type == STARTED) {
                update_time(received.s_header.s_local_time);
            }
        }
    }
    started.s_header.s_local_time = get_lamport_time();
    if (current != parent) {
        send(&id, 0, &started);
    }
    logEvent(LOG_ALL_STARTED, id, id_parent, current_amount,started.s_header.s_local_time );
}

void work() {
    if (current == parent) {

    } else {
        printf("reply %d %d %d %d\n", queue.replies[0],queue.replies[1],queue.replies[2],queue.replies[3]);
//        printf("done %d %d %d %d\n", queue.done[0], queue.done[1], queue.done[2], queue.done[3]);
        int n = id * 5;
        for (int i = 0; i<n; i++) {
            if (is_mutex)
                request_cs(NULL);
            logEvent(LOG_LOOP, id, i+1, n, time_clock);
            if (is_mutex)
                release_cs(NULL);
        }
    }
//    logEvent(LOG_ALL_DONE, id, id_parent, current_amount, time_clock);
}

// TODO check
void end() {
    if (parent != current) {
        Message finished = createMessage(MESSAGE_MAGIC, id, 0, current_amount, DONE);
        send_multicast(&id, &finished);
    }



//    Message received;
//    while (entered_mutex() != 1) {
//        int from;
//        for (int i = 1; i <= process_count; i++) {
//            if (i != id) {
//                int value = receive(&id, i, &received);
//                if (value == 0) {
//                    update_time(received.s_header.s_local_time);
//                    from = i;
//                }
//            }
//        }
//        if (from != 0) {
//            switch (received.s_header.s_type) {
//                case CS_REQUEST: {
//                    Message response = createMessage(MESSAGE_MAGIC, 0, 0, 0, CS_REPLY);
//                    Info new_info;
//                    new_info.id = from;
//                    new_info.time = received.s_header.s_local_time;
//                    push(new_info);
//                    fprintf(EventsLog, "%d: process %i CS_REQUEST from %i\n", time_clock, id, from);
//                    fflush(EventsLog);
//                    send(&id, from, &response);
//                    break;
//                }
//                case CS_REPLY:
//                    set_reply(from);
//                    fprintf(EventsLog, "%d: process %i CS_REPLY from %i\n", time_clock, id, from);
//                    fflush(EventsLog);
//                    break;
//                case CS_RELEASE:
//                    if (queue.info[0].id == from) {
//                        pop();
//                    }
//                    fprintf(EventsLog, "%d: process %i CS_RELEASE from %i\n", time_clock, id, from);
//                    fflush(EventsLog);
//                    break;
////            case DONE:
////                set_done(from);
////                fprintf(EventsLog, "%d: process %i DONE from %i",time_clock, id, from);
////                fflush(EventsLog);
//                default:
//                    fprintf(EventsLog, "%d: process %i type=%hd from %i\n", time_clock, id, received.s_header.s_type,
//                            from);
//                    fflush(EventsLog);
//                    break;
//            }
//        }
//        switch (received.s_header.s_type) {
//            case DONE:
//                set_done(from);
//                fprintf(EventsLog, "%d: process %i DONE from %i",time_clock, id, from);
//                fflush(EventsLog);
//        }
//    }



    Message received;
    for (local_id i = 1; i <= process_count; i++) {
        if (i != id) {
            int error = receive(&id, i, &received);
            if (error == 1) {
                i--;
            } else if (error == 0 && received.s_header.s_type != DONE){
                i--;
                update_time(received.s_header.s_local_time);
            }
            if (error == 0 || received.s_header.s_type == DONE) {
                update_time(received.s_header.s_local_time);
            }
        }
    }
    logEvent(LOG_ALL_DONE, id, id_parent, current_amount, received.s_header.s_local_time);
//    showHistory();
}
