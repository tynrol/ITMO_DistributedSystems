#ifndef PA2_HANDLER_H
#define PA2_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "banking.h"
#include "ipc.h"
#include "history.h"
#include "utils.h"
#include "pipes.h"
#include "pa2345.h"

extern local_id process_count;
extern local_id id;
extern local_id id_parent;
extern pid_t current;
extern pid_t parent;
extern balance_t current_amount;
extern timestamp_t last_state_time;

void init(int process_count, balance_t balance[]);
void start();
void work();
void end();

#endif //PA2_HANDLER_H
