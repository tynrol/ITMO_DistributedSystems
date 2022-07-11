#include "log.h"

FILE *EventsLog = NULL;
FILE *PipesLog = NULL;

void openLog() {
    EventsLog = fopen(events_log, "w");
    PipesLog = fopen(pipes_log, "w");
}

void closeLog() {
    fclose(EventsLog);
    fclose(PipesLog);
}

void logEvent(LogFormat format, local_id src, local_id dst, balance_t balance) {
    switch (format) {
        case LOG_STARTED:
            fprintf(EventsLog, log_started_fmt, get_physical_time(), src, getpid(), getppid(), balance);
            break;
        case LOG_DONE:
            fprintf(EventsLog, log_done_fmt, get_physical_time(), src, balance);
            break;
        case LOG_ALL_STARTED:
            fprintf(EventsLog, log_received_all_started_fmt, get_physical_time(), src);
            break;
        case LOG_ALL_DONE:
            fprintf(EventsLog, log_received_all_done_fmt, get_physical_time(), src);
            break;
        case LOG_TRANSFER_IN:
            fprintf(EventsLog, log_transfer_in_fmt, get_physical_time(), src, balance, dst);
            break;
        case LOG_TRANSFER_OUT:
            fprintf(EventsLog, log_transfer_out_fmt, get_physical_time(), src, balance, dst);
            break;
        case LOG_LOOP:
            fprintf(EventsLog, log_loop_operation_fmt, src, 1, 666);
            break;
        case LOG_STOP:
            fprintf(EventsLog, "%d: process %d requested stop\n", get_physical_time(), src);
            break;
        case LOG_ACK:
            fprintf(EventsLog, "%d: process %d ack from %d\n", get_physical_time(), src, dst);
            break;
        case LOG_COPIED:
            fprintf(EventsLog, "%d: process %d copied history\n", get_physical_time(), src);
            break;
    }

    fflush(EventsLog);
}

void logPipe(PipeFormat format, local_id src, local_id dst, int size){
    switch (format) {
        case PIPE_STARTED:
            fprintf(PipesLog, "%d: pipe %i -> %i started\n", get_physical_time(), src, dst);
            fflush(PipesLog);
        case PIPE_FAIL:
            fprintf(PipesLog, "%d: pipe %i -> %i failed\n", get_physical_time(), src, dst);
        case PIPE_FAIL_NONBLOCK:
            fprintf(PipesLog, "%d: pipe %i -> %i failed nonblock state\n", get_physical_time(), src, dst);
        case PIPE_SEND:
            fprintf(PipesLog,  "%d: process %1d send %2d bytes to process %1d\n" , get_physical_time(), src, size, dst);
            fflush(PipesLog);
            break;
        case PIPE_RECEIVE:
            fprintf(PipesLog,  "%d: process %1d received %2d bytes from process %1d\n" , get_physical_time(), src, size, dst);
            fflush(PipesLog);
            break;
        case PIPE_ERROR:
            fprintf(PipesLog,  "%d: process %1d has error %d\n" , get_physical_time(), src, size);
            break;
        case PIPE_READ_AGAIN:
            fprintf(PipesLog,  "%d: process %1d has to read again %d\n" , get_physical_time(), src, size);
            break;
    }
}
