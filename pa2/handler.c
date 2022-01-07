#include "handler.h"

BalanceHistory all;
Mesh *mesh;

void process(int process_count, balance_t balance[]) {
    openLogEvent();
    openLogPipe();
    printf("PHYSICAL TIME %d\n", get_physical_time());
    initMesh(process_count + 1, balance);
    forkProcesses();
}

void initMesh(int process_count, balance_t balance[]) {
    mesh = (Mesh *) malloc(sizeof(Mesh));
    mesh->process_count = process_count;
    mesh->current_id = getpid();
    for (int i = 0; i < mesh->process_count; i++) {
        BalanceState *state = (BalanceState *) malloc(sizeof(BalanceState));
        state->s_balance = balance[i];
        state->s_balance_pending_in = 0;
        state->s_time = get_physical_time();
//        printf("CHECK STATE %d, %d, %d\n", i, state->s_balance, state->s_time);
        mesh->balanceStates[i] = state;
        for (int j = 0; j < mesh->process_count; j++) {
            if (i != j) {
                mesh->pipes[i][j] = (PipeChannel *) malloc(sizeof(PipeChannel));
            }
        }
    }
    openPipes();
}

void openPipes() {
    int fdPipes[2];
    for (int i = 0; i < mesh->process_count; i++) {
//        printf("CHECK MESH %d, %d, %d\n", i, mesh->balanceStates[i]->s_balance, mesh->balanceStates[i]->s_time);
        for (int j = 0; j < mesh->process_count; j++) {
            if (i != j) {
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

void closePipes() {
    for (int i = 0; i < mesh->process_count; i++) {
        for (int j = 0; j < mesh->process_count; j++) {
            if (i != j && i != mesh->current_id) {
                close(mesh->pipes[i][j]->fdRead);
                close(mesh->pipes[i][j]->fdWrite);
//                logPipe(CLOSED, mesh->current_id);
            }
        }
    }
}

void closeLinePipes() {
    for (int i = 0; i < mesh->process_count; i++) {
        if (i != mesh->current_id) {
            close(mesh->pipes[mesh->current_id][i]->fdRead);
            close(mesh->pipes[mesh->current_id][i]->fdWrite);
//            logPipe(CLOSED, mesh->current_id);
        }
    }
}


void forkProcesses() {
    pid_t pid;
    for (int i = 1; i < mesh->process_count; i++) {
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

void work(ForkStatus status) {
    closePipes();
    if (status == CHILD) {
        waitEvent(EVENT_STARTED);
        serve();
        waitEvent(EVENT_RECV_ALL_DONE);
        closeLinePipes();
    } else if (status == PARENT) {
        waitEvent(EVENT_RECV_ALL_STARTED);
        
        // bank_robbery(mesh, mesh->process_count-1);
        transfer(mesh, 2, 1, 5);

        //send stop
        sleep(5);
        Message message = createMessage(MESSAGE_MAGIC, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, STOP, get_physical_time());
        if (send_multicast(mesh, &message) != 0) {
            printf("Cannot send multicast stop\n");
            exit(1);
        }

        waitEvent(EVENT_RECV_ALL_DONE);
        //history
        int value = 666;
        while (value > 0) {
            value = wait(NULL);
            // printf("Not everything is closed %d\n", value);
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

void waitEvent(EventStatus status) {
    Message *msg = (Message *) malloc(sizeof(Message));
    Message send;
    int flag;
    timestamp_t time_t = get_physical_time();
    switch (status) {
        case EVENT_STARTED:
            send = createMessage(MESSAGE_MAGIC, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, STARTED, time_t);
            if (send_multicast(mesh, &send) != 0) {
                printf("Cant send multicast");
                exit(1);
            }
            receive_any(mesh, msg);
            logEvent(EVENT_RECV_ALL_STARTED, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, time_t,
                     0);
            break;
        case EVENT_DONE:
            send = createMessage(MESSAGE_MAGIC, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, DONE, time_t);
            if (send_multicast(mesh, &send) != 0) {
                printf("Cant send multicast");
                exit(1);
            }
            receive_any(mesh, msg);
            logEvent(EVENT_RECV_ALL_DONE, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, time_t,
                     0);
            break;
        case EVENT_RECV_ALL_STARTED:
            flag = 0;
            for (local_id i = 0; i < mesh->process_count; i++) {
                if (i != mesh->current_id) {
                    if (receive(mesh, i, msg) != 0) {
                        flag = 1;
                    }
                    if (msg->s_header.s_type != STARTED) {
                        flag = 1;
                    }
                }
            }

            logEvent(EVENT_RECV_ALL_STARTED, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, time_t,
                     0);
            break;
        case EVENT_RECV_ALL_DONE:
            flag = 0;
            for (local_id i = 0; i < mesh->process_count; i++) {
                if (i != mesh->current_id) {
//                    printf("COUNT %d, %d\n", i, mesh->process_count);
                    if (receive(mesh, i, msg) != 0) {
                        flag = 1;
                    }
                    if (msg->s_header.s_type != DONE) {
                        flag = 1;
                    }
                }
            }
            logEvent(EVENT_RECV_ALL_DONE, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, time_t,
                     0);
            break;
        default:
            break;
    }
}

void serve() {
    Message *msg = (Message *) malloc(sizeof(Message));
    int exit = 0;
    while (exit != 1) {
        for (local_id i = 0; i < mesh->process_count; i++) {
            if (i != mesh->current_id) {
                int value = receive(mesh, i, msg);
                if(value!=0) {
                    // exit = 1;
                    // printf("SHUYALI %d\n", value);
                }
                
                fflush(stdout);
                switch (msg->s_header.s_type) {
                    case TRANSFER:
                        // printf("RECEIVED TRANSFER \n");
                        fflush(stdout);
                        handleTransfer(msg);
                        break;
                    case STOP:
                        // printf("RECEIVED STOP\n");
//                      TODO: SEND HISTORY TO PARENT
                        exit = 1;
                        break;
                    default:
                        // printf("DEFAULT STATUS %d\n", msg->s_header.s_type);
                        continue;
                }
            }
        }
    }
    // printf("SERVING ENDED\n");
}

void handleTransfer(Message *message) {
    TransferOrder order;
    memcpy(&order, message->s_payload, sizeof(TransferOrder));
    // printf("--------2------ %d, %d, %d, %d\n",mesh->current_id, order.s_src, order.s_dst, order.s_amount);

    if (mesh->current_id == order.s_src) {
        mesh->balanceStates[mesh->current_id]->s_balance -= order.s_amount;
        mesh->balanceStates[mesh->current_id]->s_time = get_physical_time();
        send(mesh, order.s_dst, message);
        logEvent(EVENT_TRANSFER_OUT, order.s_src, order.s_amount, get_physical_time(), order.s_dst);
    } else if (mesh->current_id == order.s_dst) {
        mesh->balanceStates[mesh->current_id] += order.s_amount;
        mesh->balanceStates[mesh->current_id]->s_time = get_physical_time();
        Message msg = createMessage(MESSAGE_MAGIC, mesh->current_id, order.s_amount, ACK, get_physical_time());
        send(mesh, 0, &msg);
        logEvent(EVENT_TRANSFER_IN, order.s_dst, order.s_amount, get_physical_time(), order.s_src);
    } else {
        printf("WRONG CLIENT\n");
    }    
}
