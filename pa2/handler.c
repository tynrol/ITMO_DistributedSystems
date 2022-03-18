#include "handler.h"

BalanceHistory all;
Mesh *mesh;

void process(int process_count, balance_t balance[]) {
    openLogEvent();
    openLogPipe();
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
        for (int j = 0; j < mesh->process_count; j++) {
            if (i != j) {
                pipe(fdPipes);
                if (fcntl(fdPipes[0], F_SETFL, O_NONBLOCK) < 0 && fcntl(fdPipes[1], F_SETFL, O_NONBLOCK) < 0)
                    exit(2);
                mesh->pipes[j][i]->fdRead = fdPipes[0];
                mesh->pipes[i][j]->fdWrite = fdPipes[1];
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
            }
        }
    }
}

void closeLinePipes() {
    for (int i = 0; i < mesh->process_count; i++) {
        if (i != mesh->current_id) {
            close(mesh->pipes[mesh->current_id][i]->fdRead);
            close(mesh->pipes[mesh->current_id][i]->fdWrite);
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
        start();
        childServe();
        end();

        closeLinePipes();
        exit(0);
    } else if (status == PARENT) {
        start();
        parentServe();

        int value = 666;
        while (value > 0) {
            value = wait(NULL);
        }

        closePipes();
        closeLogEvent();
        closeLogPipe();

        exit(0);
    } else {
        printf("Wrong process status");
        exit(3);
    }
}

void start() {
    Message started = createMessage(MESSAGE_MAGIC,
                                    mesh->current_id,
                                    0,
                                    mesh->balanceStates[mesh->current_id]->s_balance,
                                    STARTED,
                                    get_physical_time());
//    this shit doesnt have to send to host, why dont i have separate functions
//    send_multicast(mesh, started);
//    literally have to copy
    for (local_id i = 1; i < mesh->process_count; i++) {
        if (i != mesh->current_id) {
            if (send(mesh, i, &started) != 0) {
                printf("Cant send start message to other children\n");
                exit(4);
            }
        }
    }

    Message recv;
    for (local_id i = 1; i<mesh->process_count; i++) {
        if (i != mesh->current_id) {
            if (receive(mesh, i, &recv) == 0 && recv.s_header.s_type != STARTED) {
                printf("Wrong message received by host on start\n");
                exit(2);
            }
        }
    }

    if (mesh->current_id == 0) {
        logEvent(EVENT_RECV_ALL_STARTED, 0, 0, get_physical_time(), 0);
    } else {
        send(mesh, 0, &started);
//        already started in create start message dont remember if all started needed
        logEvent(EVENT_RECV_ALL_STARTED, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, get_physical_time(),0);
    }
}

void end() {
    Message stopped = createMessage(MESSAGE_MAGIC,
                                    mesh->current_id,
                                    0,
                                    mesh->balanceStates[mesh->current_id]->s_balance,
                                    STOP,
                                    get_physical_time());
    send_multicast(mesh, &stopped);

    Message recv;
    for (local_id i = 1; i<mesh->process_count; i++) {
        if (i != mesh->current_id) {
            int value = receive(mesh, i, &recv) == 0;
            if (value == 0 && recv.s_header.s_type != STOP) {
//                printf("%d: host process %d received type %d\n", get_physical_time(), mesh->current_id, recv.s_header.s_type);
                i--;
            }
        }
    }

    if (mesh->current_id == 0) {
        logEvent(EVENT_RECV_ALL_DONE, 0, 0, get_physical_time(), 0);
    } else {
        //        already started in create start message dont remember if all started needed
        logEvent(EVENT_RECV_ALL_DONE, mesh->current_id, mesh->balanceStates[mesh->current_id]->s_balance, get_physical_time(),0);
        send(mesh, 0, &stopped);
    }

    // TODO write to a history whatever
}


void childServe() {
    Message ack;
    Message recv;
    TransferOrder order;
    int exit = 0;
    int value;
    while(exit == 0) {
        while ((value = receive_any(mesh, &recv)) == -1) {
            if (value == 0)
            switch (recv.s_header.s_type) {
                case STOP:
                    // TODO add to history
                    exit = -1;
                    break;
                case TRANSFER:
                    memcpy(&order, recv.s_payload, sizeof(TransferOrder));
                    if (order.s_dst == mesh->current_id) {
                        // TODO add to history
                        logEvent(EVENT_TRANSFER_IN, order.s_dst, order.s_amount, get_physical_time(), order.s_src);
                        mesh->balanceStates[mesh->current_id] += order.s_amount;
                        ack = createMessage(MESSAGE_MAGIC, 0, 0, 0, ACK, get_physical_time());
                        send(mesh, 0, &ack);
                    } else if (order.s_src == mesh->current_id) {
                        // TODO add to history
                        logEvent(EVENT_TRANSFER_OUT, order.s_src, order.s_amount, get_physical_time(), order.s_dst);
                        mesh->balanceStates[mesh->current_id] -= order.s_amount;
                        ack = createMessage(MESSAGE_MAGIC, 0, 0, 0, ACK, get_physical_time());
                        send(mesh, 0, &ack);
                    }
                    break;
//            default:
//                printf("%d: process %d received type %d\n", get_physical_time(), mesh->current_id, recv.s_header.s_type);
            }
        }
    }
}

void parentServe() {
    bank_robbery(mesh, mesh->process_count-1);
//    end();
}
