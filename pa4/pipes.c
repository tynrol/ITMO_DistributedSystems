#include "pipes.h"

#define max_processes 10
PipeChannel pipes[max_processes][max_processes];

void openPipes(){
    for(int i = 0; i <= process_count; i++){
        for(int j = 0; j <= process_count; j++){
            if(i != j){
                int value;
                value = pipe(pipes[i][j].fd);
                if(value == -1) {
                    logPipe(PIPE_FAIL, i, j, 0, get_lamport_time());
                    exit(-1);
                }
                value = fcntl(pipes[i][j].fd[0], F_SETFL, O_NONBLOCK);
                if(value < 0 ){
                    logPipe(PIPE_FAIL_NONBLOCK, i, j, 0, get_lamport_time());
                    exit(-2);

                }
                value = fcntl(pipes[i][j].fd[1], F_SETFL, O_NONBLOCK);
                if(value < 0) {
                    logPipe(PIPE_FAIL_NONBLOCK, i, j, 0, get_lamport_time());
                    exit(-2);

                }
                logPipe(PIPE_STARTED, i, j, 0, get_lamport_time());

            }
        }
    }
}

void closePipes(){
    for(int i = 0; i <= process_count; i++){
        for(int j = 0; j <= process_count; j++){
            if(i != id && i != j){
                close(pipes[i][j].fd[1]);
            }
            if(j != id && i != j){
                close(pipes[i][j].fd[0]);
            }
        }
    }
}

void closeLinePipes(){
    for(local_id j = 0; j < process_count; j++){
        if(j != id) {
            close(pipes[id][j].fd[1]);
            close(pipes[j][id].fd[0]);
        }
    }
}

