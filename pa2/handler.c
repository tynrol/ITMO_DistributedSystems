#include "handler.h"

BalanceHistory all;
Mesh *mesh;

void process(int process_count, balance_t balance[]) {
    openLogEvent();
    openLogPipe();
    initMesh(process_count+1, balance);
    forkProcesses();
}

void initMesh(int process_count, balance_t balance[]) {
    mesh = (Mesh *)malloc(sizeof(Mesh));
    mesh->process_count = process_count;
    mesh->current_id = getpid();
    for(int i = 0; i<mesh->process_count; i++){
        BalanceState *state = (BalanceState *)malloc(sizeof(BalanceState));
        state->s_balance = balance[i];
        state->s_balance_pending_in = 0;
        state->s_time = get_physical_time();
//        printf("CHECK STATE %d, %d, %d\n", i, state->s_balance, state->s_time);
        mesh->balanceStates[i] = state;
        for(int j = 0; j<mesh->process_count; j++){
            if(i!=j){
                mesh->pipes[i][j] = (PipeChannel *)malloc(sizeof(PipeChannel));
            }
        }
    }
    openPipes();
}

void openPipes(){
    int fdPipes[2];
    for(int i = 0; i<mesh->process_count; i++){
//        printf("CHECK MESH %d, %d, %d\n", i, mesh->balanceStates[i]->s_balance, mesh->balanceStates[i]->s_time);
        for(int j = 0; j<mesh->process_count; j++){
            if(i!=j){
                pipe(fdPipes);
                if (fcntl(fdPipes[0], F_SETFL, O_NONBLOCK) < 0 && fcntl(fdPipes[1], F_SETFL, O_NONBLOCK) < 0)
                    exit(2);
                mesh->pipes[j][i]->fdRead = fdPipes[0];
                mesh->pipes[i][j]->fdWrite = fdPipes[1];
//                logPipe(OPEN, mesh->current_id);
            }
        }
    }
}

void closePipes(){
    for(int i = 0; i<mesh->process_count; i++){
        for(int j = 0; j<mesh->process_count; j++){
            if(i!=j & i!=mesh->current_id){
                close(mesh->pipes[i][j]->fdRead);
                close(mesh->pipes[i][j]->fdWrite);
//                logPipe(CLOSED, mesh->current_id);
            }
        }
    }
}

void closeLinePipes(){
    for(int i = 0;i<mesh->process_count;i++){
        if(i!=mesh->current_id){
            close(mesh->pipes[mesh->current_id][i]->fdRead);
            close(mesh->pipes[mesh->current_id][i]->fdWrite);
//            logPipe(CLOSED, mesh->current_id);
        }
    }
}


void forkProcesses(){
    pid_t pid;
    for(int i = 1;i<mesh->process_count;i++){
        pid = fork();
        switch (pid) {
            case -1:
                printf("Cannot create fork process");
                break;
            case 0:
                mesh->current_id = i;
                work(CHILD);
                exit(0);
                break;
            default:
                break;
        }
    }
    mesh->current_id = 0;
    work(PARENT);
}

void work(ForkStatus status){
    closePipes();
    if(status==CHILD) {
        waitEvent(EVENT_STARTED);

        waitEvent(EVENT_RECV_ALL_DONE);
        closeLinePipes();
    } else if(status==PARENT){
        waitEvent(EVENT_RECV_ALL_STARTED);

        bank_robbery(mesh, mesh->process_count);
//        history

        waitEvent(EVENT_RECV_ALL_DONE);
        int value = 666;
        while(value > 0) {
            value = wait(NULL);
//            printf("Not everything is closed %d\n", val);
        }
        closePipes();
        closeLogEvent();
        closeLogPipe();
        exit(0);
    } else {
        printf("Wrong process status");
        exit(1);
    }
}

void waitEvent(EventStatus status){
    Message msg;
    Message send;
    switch (status) {
        case EVENT_STARTED:
            send = createMessage(MESSAGE_MAGIC, mesh->current_id, STARTED, (unsigned long)time(NULL));
            if(send_multicast(mesh, &send)!=0){
                printf("Cant send multicast");
                exit(1);
            }
            receive_any(mesh, &msg);
            logEvent(EVENT_RECV_ALL_STARTED, mesh->current_id);
            break;
        case EVENT_DONE:
            send = createMessage(MESSAGE_MAGIC, mesh->current_id, DONE, (unsigned long)time(NULL));
            if(send_multicast(mesh, &send)!=0){
                printf("Cant send multicast");
                exit(1);
            }
            receive_any(mesh, &msg);
            logEvent(EVENT_RECV_ALL_DONE, mesh->current_id);
            break;
        case EVENT_RECV_ALL_STARTED:
            receive_any(mesh, &msg);
//            printf("handler switch %s\n", msg.s_payload);
            logEvent(EVENT_RECV_ALL_STARTED, mesh->current_id);
            break;
        case EVENT_RECV_ALL_DONE:
            receive_any(mesh, &msg);
            logEvent(EVENT_RECV_ALL_DONE, mesh->current_id);
            break;
    }
}
