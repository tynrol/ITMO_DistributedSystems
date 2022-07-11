#include "handler.h"
#include <stdlib.h>
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include "pa2345.h"


local_id id_parent = 0;
local_id id = 0;
local_id process_count = 0;
pid_t parent = 0;
pid_t current = 0;
int is_mutex = 0;


void init(){
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
            logEvent(LOG_STARTED, id, id_parent, current, time_clock);
            break;
        }
    }
    initQueue();
    closePipes();

    start();
    work();
    end();

    closeLinePipes();
    if (parent == current) {

        int status;
        for (int i = 1; i <= process_count; i++) {
            if (wait(&status) == -1) {
                printf("Error while ending child\n");
                exit(-10);
            }
        }
        printf("\nFinished successfully\n");
    }
}



void start() {
    Message started = createMessage(MESSAGE_MAGIC, id, 0, current, STARTED);
    if (current != parent) {
        send_multicast(&id, &started);
    }
    Message received;
    for (local_id i = 1; i <= process_count; i++) {
        if (i != id) {
            int error = receive_with_time(&id, i, &received);
            if (error == 0 && received.s_header.s_type != STARTED) {
                exit(-1);
            } else if (error == 1) {
                i--;
            }
        }
    }

    logEvent(LOG_ALL_STARTED, id, id_parent, current,received.s_header.s_local_time );
}

void work() {
    if (current == parent) {

    } else {
//        printf("reply %d %d %d %d\n", queue.replies[0],queue.replies[1],queue.replies[2],queue.replies[3]);
        int n = id * 5;
        for (int i = 0; i<n; i++) {
            if (is_mutex)
                request_cs(NULL);
            logEvent(LOG_LOOP, id, i+1, n, time_clock);
            if (is_mutex)
                release_cs(NULL);
        }
    }
    logEvent(LOG_ALL_DONE, id, id_parent, current, time_clock);
}

void end() {
    if (parent != current) {
        Message finished = createMessage(MESSAGE_MAGIC, id, 0, current, DONE);
        send_multicast(&id, &finished);
    }
    Message received;
    while (exit_mutex() != 1) {
        local_id from = get(&received);
        if (current != parent)
            if (from > 0) {
                switch (received.s_header.s_type) {
                    case CS_REQUEST: {
                        Message response = createMessage(MESSAGE_MAGIC, 0, 0, 0, CS_REPLY);
                        Info new_info;
                        new_info.id = from;
                        new_info.time = received.s_header.s_local_time;
                        push(new_info);
                        fprintf(EventsLog, "%d: process %i CS_REQUEST from %i\n", time_clock, id, from);
                        fflush(EventsLog);
                        send(&id, from, &response);
                        break;
                    }
                    case CS_REPLY: {
                        set_reply(from);
                        fprintf(EventsLog, "%d: process %i CS_REPLY from %i\n", time_clock, id, from);
                        fflush(EventsLog);
                        break;
                    }
                    case CS_RELEASE: {
                        if (queue.info[0].id == from) {
                            pop();
                        }
                        fprintf(EventsLog, "%d: process %i CS_RELEASE from %i\n", time_clock, id, from);
                        fflush(EventsLog);
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        if (from > 0) {
            switch (received.s_header.s_type) {
                case DONE: {
                    set_done(from);
                    fprintf(EventsLog, "%d: process %i DONE from %i",time_clock, id, from);
                    fflush(EventsLog);
                }
            }
        }
    }
    logEvent(LOG_ALL_DONE, id, id_parent, current, received.s_header.s_local_time);
}
