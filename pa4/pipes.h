#ifndef PA1_1_PIPES_H
#define PA1_1_PIPES_H

#include "log.h"
#include "ipc.h"
#include "banking.h"
#include "handler.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct{
    int fd[2];
} PipeChannel;

#define max_processes 10
extern PipeChannel pipes[max_processes][max_processes];

extern FILE * PipesLog;
extern FILE * EventsLog;

void openPipes();
void closePipes();
void closeLinePipes();

#endif //PA1_1_PIPES_H
