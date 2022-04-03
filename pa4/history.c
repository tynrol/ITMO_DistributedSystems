#include "history.h"

balance_t ** balances = NULL;
AllHistory allHistory;
BalanceHistory balanceHistory;

void initHistory() {
    balanceHistory.s_id = id;
    balanceHistory.s_history_len = 1;
    BalanceState state = createBalanceState(current_amount,0,0);
    balanceHistory.s_history[0] = state;
    last_state_time = 1;
}

void addToHistory(timestamp_t time, balance_t amount, balance_t pending) {
    for (timestamp_t i = last_state_time; i < time; i++) {
        BalanceState state = createBalanceState(balanceHistory.s_history[i - 1].s_balance, i, 0);
        balanceHistory.s_history[i] = state;
    }
    BalanceState new_state = createBalanceState(amount, time, pending);
    balanceHistory.s_history[time] = new_state;
    balanceHistory.s_history_len = time + 1;
    last_state_time = (timestamp_t) (time + 1);
}

void copyHistory(char *payload) {
    memcpy(payload, &(balanceHistory.s_id), sizeof(local_id));
    memcpy(payload + sizeof(local_id), &(balanceHistory.s_history_len), sizeof(uint8_t));
    memcpy(payload + sizeof(local_id) + sizeof(uint8_t), balanceHistory.s_history,
           balanceHistory.s_history_len * sizeof(BalanceState));
}

void showHistory() {
    Message message = createMessage(MESSAGE_MAGIC, id, id_parent, current_amount, BALANCE_HISTORY);
    message.s_header.s_payload_len = sizeof(local_id) + sizeof(uint8_t) + sizeof(BalanceState) * (balanceHistory.s_history_len);
    copyHistory(message.s_payload);

    if (parent != current) {
        send(&id, id_parent, &message);
    } else {
        Message received;
        for (local_id i = 1; i <= process_count; i++) {
            receive_all(&received, i, id);
            if(received.s_header.s_type == BALANCE_HISTORY){
                logEvent(LOG_COPIED, id, id_parent, current_amount, received.s_header.s_local_time);
                memcpy(allHistory.s_history + allHistory.s_history_len, received.s_payload, sizeof(BalanceHistory));
                allHistory.s_history_len++;
            }
        }
        print_history(&allHistory);
    }
}
