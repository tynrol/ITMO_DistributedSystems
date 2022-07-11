#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "banking.h"
#include "handler.h"

void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    Message message = createMessage(MESSAGE_MAGIC, src, dst, amount,TRANSFER);
    send(&id, src, &message);
    receive_all(&message, dst, id);
    if (message.s_header.s_type != ACK) {
        exit(-1);
    } else {
        update_time(message.s_header.s_local_time);
        logEvent(LOG_ACK, src, dst, amount, message.s_header.s_local_time);
    }
}

int main(int argc, char *argv[]) {
    balances = malloc(sizeof(balance_t *));
    int c;
    while ((c = getopt(argc, argv, "p:")) != -1) {
        switch(c){
            case 'p':
                process_count = (local_id) atoi(optarg);
                break;
            default:
                break;
        }
    }
    *balances = malloc(sizeof(int) * process_count);
    for(int i = 0; i<process_count;i++) {
        (*balances)[i] = (balance_t) atoi(argv[i+3]);
    }

    printf("Process count: %d\n", process_count);
    for (int i = 0; i < process_count; i++) {
        printf("Start balance %d: $ %d\n", i, (*balances)[i]);
    }

    init(process_count, *balances);
    return 0;
}
