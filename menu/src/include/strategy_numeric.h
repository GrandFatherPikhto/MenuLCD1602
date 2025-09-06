#ifndef STRATEGY_NUMERIC
#define STRATEGY_NUMERIC

#include "menu.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct numeric_data {
    int32_t *value_ptr;          // Указатель на значение
    int32_t min_value;           // Минимальное значение
    int32_t max_value;           // Максимальное значение
    uint8_t current_factor_idx;  // Текущий индекс множителя
    void (*change_callback)(int32_t new_value); // Callback при изменении
} numeric_data_t;

/**
 * @brief Стратегия для числовых параметров с изменяемым множителем
 * node->userdata должно указывать на numeric_data_t
 */
extern const menu_node_strategy_t MENU_NUMERIC_STRATEGY;

#ifdef __cplusplus
}
#endif


#endif // STRATEGY_NUMERIC 