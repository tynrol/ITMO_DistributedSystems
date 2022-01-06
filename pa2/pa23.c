#include <getopt.h>
#include "handler.h"

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount) {

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
//        printf("START BALANCE %d = %d\n", i, balance[i]);
    }
    process(process_count, balance);
//    print_history(all);

    return 0;
}
