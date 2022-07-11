#ifndef PA1_1_PHASES_H
#define PA1_1_PHASES_H


#include "banking.h"
#include "pipes.h"
#include "cs_shit.h"
#include "queue.h"
#include <sys/wait.h>

extern int is_mutex;
extern local_id process_count;
extern local_id id;
extern local_id id_parent;
extern pid_t current;
extern pid_t parent;
extern pid_t current;

void init();
void start();
void work();
void end();

#endif //PA1_1_PHASES_H
