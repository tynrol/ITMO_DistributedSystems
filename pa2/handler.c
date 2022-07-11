#include "handler.h"


local_id process_count = 0;
local_id id = 0;
local_id id_parent = 0;
pid_t current = 0;
pid_t parent = 0;
balance_t current_amount = 0;
timestamp_t last_state_time = 0;

void init(int process_count, balance_t balance[]) {
    parent = getpid();
    current = parent;

    openLog();
    openPipes();

    pid_t pid;
    for (int i = 1; i <= process_count; i++) {
        current_amount = (*balances)[i - 1];
        pid = fork();
        if (pid == -1) {
            printf("Error on creating child: %d\n", i);
            exit(-10);
        }

        if (pid == 0) {
            id = (local_id) i;
            current = pid;
            initHistory();
            logEvent(LOG_STARTED, id, id_parent, current_amount);
            break;
        }
    }
    closePipes();

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
        }
    }
    if (current != parent) {
        send(&id, 0, &started);
    }
    logEvent(LOG_ALL_STARTED, id, id_parent, current_amount);
}

void work() {
    if (current == parent) {
        bank_robbery(NULL, (local_id) (process_count));
        Message stop = createMessage(MESSAGE_MAGIC, id, id_parent, current_amount, STOP);
        send_multicast(&id, &stop);
    } else {
        Message received;
        Message ack = createMessage(MESSAGE_MAGIC, id, id_parent, current_amount, ACK);
        TransferOrder order;
        int error;
        int exit = 0;
        while (exit == 0) {
            while ((error = receive_any(&id, &received)) == 1);
            if (error != -1) {
                switch (received.s_header.s_type) {
                    case STOP:
                        exit = 1;
                        addToHistory(received.s_header.s_local_time, current_amount);
                        break;
                    case TRANSFER:
                        memcpy(&order, received.s_payload, sizeof(TransferOrder));
                        if (order.s_dst == id) {
                            timestamp_t time = get_physical_time();
                            transaction_in(time, order.s_amount, order.s_src);
                            ack.s_header.s_local_time = time;
                            send(&id, id_parent, &ack);
                        } else if (order.s_src == id) {
                            timestamp_t time = get_physical_time();
                            transaction_out(time, order.s_amount, order.s_dst);
                            received.s_header.s_local_time = time;
                            send(&id, order.s_dst, &received);
                        }
                        break;
                }
            }
        }
    }
    logEvent(LOG_ALL_DONE, id, id_parent, current_amount);
}

void end() {
    if (parent != current) {
        Message finished = createMessage(MESSAGE_MAGIC, id, 0, current_amount, DONE);
        send_multicast(&id, &finished);
    }
    Message received;
    for (local_id i = 1; i <= process_count; i++) {
        if (i != id) {
            int error = receive(&id, i, &received);
            if (error == 1 || received.s_header.s_type != DONE) {
                i--;
            }
        }
    }
    logEvent(LOG_ALL_DONE, id, id_parent, current_amount);
    showHistory();
}
