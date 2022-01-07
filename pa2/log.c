#include "log.h"

int fdEventsLog;
int fdPipesLog;

void openLogEvent() {
    fdEventsLog = open(events_log, O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0760);
}

void openLogPipe() {
    fdPipesLog = open(pipes_log, O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0760);
}

void closeLogEvent() {
    close(fdEventsLog);
}

void closeLogPipe() {
    close(fdPipesLog);
}

char *logEvent(EventStatus status, local_id id, balance_t balance, timestamp_t timestamp, local_id foreign_id) {
    char *buf = (char *) malloc(sizeof(char) * 255);

    switch (status) {
        case EVENT_STARTED:
            sprintf(buf, log_started_fmt, timestamp, id, getpid(), getppid(), balance);
            break;
        case EVENT_RECV_ALL_STARTED:
            sprintf(buf, log_received_all_started_fmt, timestamp, id);
            break;
        case EVENT_DONE:
            sprintf(buf, log_done_fmt, timestamp, id, balance);
            break;
        case EVENT_RECV_ALL_DONE:
            sprintf(buf, log_received_all_done_fmt, timestamp, id);
            break;
        case EVENT_TRANSFER_IN:
            sprintf(buf, log_transfer_in_fmt, timestamp, id, balance, foreign_id);
            break;
        case EVENT_TRANSFER_OUT:
            sprintf(buf, log_transfer_out_fmt, timestamp, id, balance, foreign_id);
            break;
        case EVENT_STOP:
            printf("%d: process %d requested stop\n", timestamp, id);
            break;
    }

    printf(buf, 0);
    write(fdEventsLog, buf, strlen(buf));
    return buf;
}

//void logPipe(PipeStatus status, local_id id, int from, int to, int descriptor){
//    char buf[100];
//
//    switch (status) {
//        case OPEN:
//            sprintf(buf, pipe_opend_msg, id, from, to, descriptor);
//            break;
//        case CLOSED:
//            sprintf(buf, pipe_closed_msg, id, from, to, descriptor);
//            break;
//    }
//    write(fdPipesLog, buf, 100);
//}
