#include "banking.h"
#include <stdio.h>
#include "pipes.h"
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "handler.h"


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
