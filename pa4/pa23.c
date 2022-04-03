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
    const char* short_options = "p:";
    const struct option long_options[] = {
            { "mutexl", no_argument, NULL, 'm' },
            { NULL, 0, NULL, 0 }
    };

    int c;
    while ((c = getopt_long(argc,argv,short_options,long_options,NULL)) != -1){

        switch(c){
            case 'm':
                is_mutex = 1;
                break;
            case 'p':
                process_count = (local_id) atoi(optarg);
                break;
            case '?': default:
                printf("Option unknown\n");
                break;
        };
    };

    printf("Process count: %i\n", process_count);
    printf("Mutex: %d\n\n", is_mutex);

    init();
    return 0;
}
