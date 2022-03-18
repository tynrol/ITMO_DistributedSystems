#include <getopt.h>
#include "handler.h"

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {
    printf("Transfer from %d to %d $%d\n", src, dst, amount);
    Mesh *mesh = (Mesh *)parent_data;
    Message message = createMessage(MESSAGE_MAGIC, src,dst,amount,TRANSFER,get_physical_time());
    Message recv;
    send(mesh, src, &message);
    while(receive(mesh, dst, &recv) != 0 && recv.s_header.s_type != ACK) {
        receive(mesh, dst, &recv);
    }
}

int main(int argc, char * argv[]){
    int process_count = 0;
    int c;
    balance_t balance[MAX_PROCESS_COUNT];

    while ((c = getopt (argc, argv, "p:")) != -1) {
        switch(c){
            case 'p':
                process_count = atoi(optarg);
                break;
            default:
                break;
        }
    }
    for(int i = 1; i<=process_count;i++) {
        balance[i] = atoi(argv[i+2]);
    }
    process(process_count, balance);
    return 0;
}
