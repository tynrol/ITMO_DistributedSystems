#ifndef PA2_HISTORY_H
#define PA2_HISTORY_H

#include "banking.h"
#include "handler.h"
#include "pipes.h"
#include "utils.h"
#include <string.h>

extern balance_t ** balances;
extern BalanceHistory balanceHistory;
extern AllHistory allHistory;

void initHistory();
void addToHistory(timestamp_t time, balance_t amount, balance_t pending);
void copyHistory(char *payload);
void showHistory();

#endif //PA2_HISTORY_H
