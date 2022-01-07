#include <getopt.h>
#include "handler.h"

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {
    Mesh *mesh = (Mesh *)parent_data;
    TransferOrder order = createTransferOrder(src, dst, amount);
    Message message;
    Message *receiveMsg = (Message *)malloc(sizeof(Message));
    MessageHeader header;

    header.s_magic = MESSAGE_MAGIC;
    header.s_type = TRANSFER;
    header.s_local_time = get_physical_time();
    header.s_payload_len = sizeof(TransferOrder);

    message.s_header = header;
    memcpy(message.s_payload, &order, sizeof(TransferOrder));
    send(mesh, src, &message);
    int value;
    while(value != 0){
        value = receive(mesh, dst, receiveMsg);
        printf("SSSSSSSSS %d", value);
    }
    // while(receive(mesh, dst, receiveMsg)!=0 && (receiveMsg->s_header.s_type != ACK)){}
}

int main(int argc, char * argv[]){
    int process_count = 0;
    int c;
    balance_t balance[MAX_PROCESS_COUNT];

    while ((c = getopt (argc, argv, "p:")) != -1) {
        switch(c){
            case 'p':
                process_count = atoi(optarg);
                printf("START PROCESS_COUNT %d\n", process_count);
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
