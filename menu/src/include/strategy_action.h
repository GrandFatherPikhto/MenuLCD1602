#ifndef STRATEGY_ACTION_H
#define STRATEGY_ACTION_H

#include "menu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Тип callback для action-узлов
 * delta > 0 / < 0 → изменение значения
 * delta == 0 → событие "нажатие"
 */
typedef void (*action_fn_t)(int8_t delta);

/**
 * @brief Стратегия для узлов-действий
 * node->userdata должно указывать на action_fn_t
 */
extern const menu_node_strategy_t MENU_ACTION_STRATEGY;

#ifdef __cplusplus
}
#endif

#endif // STRATEGY_ACTION_H
