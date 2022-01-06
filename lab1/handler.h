#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/io.h>
#include "utils.h"

typedef enum{
    CHILD,
    PARENT,
    ERROR
} ForkStatus;

void process(int process_count);
void initMesh(int process_count);
void openPipes();
void closePipes();
void closeUnusedPipes();
void forkProcesses();
void work(ForkStatus status);
void waitEvent(EventStatus status);


