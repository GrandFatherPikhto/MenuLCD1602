#ifndef STRATEGY_TOGGLE_H
#define STRATEGY_TOGGLE_H

#include "menu.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TOGGLE_STRATEGY_FLAG_ENABLE 0x01
#define TOGGLE_STRATEGY_FLAG_DISABLE_ON_EXIT 0x02

/**
 * @brief Стратегия для узлов-переключателей (ON/OFF).
 * node->userdata должно указывать на bool.
 */
extern const menu_node_strategy_t MENU_TOGGLE_STRATEGY;

typedef struct toggle_strategy_data {
    void (*change_cb)(bool);
    uint8_t flags;
} toggle_strategy_data_t;

#ifdef __cplusplus
}
#endif

#endif // STRATEGY_TOGGLE_H
