#ifndef PA4_CS_SHIT_H
#define PA4_CS_SHIT_H

#include "queue.h"
#include "utils.h"
#include "log.h"

void reset_done();
void set_done(int i);
int is_all_done();

void reset_replies();
void set_reply(int i);
int is_all_replied();

int entered_mutex();
int release_cs(const void *self);
int request_cs(const void *self);


#endif //PA4_CS_SHIT_H
