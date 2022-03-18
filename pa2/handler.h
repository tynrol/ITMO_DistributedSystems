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

void process(int process_count, balance_t balance[]);
void initMesh(int process_count, balance_t balance[]);

void openPipes();
void closePipes();
void closeUnusedPipes();

void forkProcesses();
void work(ForkStatus status);

void childServe();
void parentServe();

void start();
void end();

void handleTransfer();
void waitEvent(EventStatus status);


