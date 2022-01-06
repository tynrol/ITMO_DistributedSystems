#include <getopt.h>
#include "handler.h"

int main(int argc, char *argv[]) {
    int process_count = 0;
    int c;
    while ((c = getopt (argc, argv, "p:")) != -1) {
        switch(c){
            case 'p':
                process_count = atoi(optarg);
                break;
            default:
                break;
        }
    }
    process(process_count);
    return 0;
}
