#ifndef PA2_PIPES_H
#define PA2_PIPES_H

#include "ipc.h"
#include "banking.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "log.h"
#include "handler.h"


typedef struct{
    int fd[2];
} PipeChannel;

#define max_processes 10
extern PipeChannel pipes[max_processes][max_processes];

void openPipes();
void closePipes();
void closeLinePipes();

#endif //PA2_PIPES_H
