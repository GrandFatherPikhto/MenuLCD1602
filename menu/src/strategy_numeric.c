// strategy_numeric.c
#include "strategy_numeric.h"
#include "strategy_default.h"
#include <stdio.h>
#include <string.h>

// Таблица множителей
static const uint8_t factors[] = {1, 2, 5, 10, 25, 50, 100};
static const int factors_count = sizeof(factors) / sizeof(factors[0]);

static void numeric_push_button(menu_handle_t *handle, menu_node_t *node) {
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    numeric_data_t *data = (numeric_data_t *)menu_node_userdata(current);
    if (!data || !data->value_ptr) return;

    // Переключаем множитель по кругу
    data->current_factor_idx = (data->current_factor_idx + 1) % factors_count;
    printf("[NUMERIC] %s: factor changed to %d\n", 
           menu_node_title(current), 
           factors[data->current_factor_idx]);
}

static void numeric_double_click_button(menu_handle_t *handle, menu_node_t *node) {
    // Долгое нажатие - сброс к минимальному значению
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    numeric_data_t *data = (numeric_data_t *)menu_node_userdata(current);
    if (!data || !data->value_ptr) return;

    *(data->value_ptr) = data->min_value;
    
    if (data->change_callback) {
        data->change_callback(*(data->value_ptr));
    }
    
    printf("[U32] %s: reset to %d\n", 
           menu_node_title(current), 
           *(data->value_ptr));
}

static void numeric_handle_delta(menu_handle_t *handle, menu_node_t *node, int8_t delta) {
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    numeric_data_t *data = (numeric_data_t *)menu_node_userdata(current);
    if (!data || !data->value_ptr) return;

    // Получаем текущий множитель
    uint8_t factor = factors[data->current_factor_idx];
    int32_t change = delta * factor;
    int32_t new_value = *(data->value_ptr) + change;

    // Ограничиваем значение
    if (new_value < data->min_value) new_value = data->min_value;
    if (new_value > data->max_value) new_value = data->max_value;

    // Применяем изменение только если оно действительно произошло
    if (new_value != *(data->value_ptr)) {
        *(data->value_ptr) = new_value;
        
        if (data->change_callback) {
            data->change_callback(new_value);
        }
        
        printf("[NUMERIC] %s: %d (%+d, factor: %d)\n", 
               menu_node_title(current), 
               new_value, change, factor);
    }
}

static void numeric_title_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    (void)handle;
    snprintf(buf, len, "%s", menu_node_title((menu_node_t *)node));
}

static void numeric_value_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    menu_node_t *current = menu_resolve_current(handle, (menu_node_t*)node);
    if (!current) return;

    numeric_data_t *data = (numeric_data_t *)menu_node_userdata(current);
    if (!data || !data->value_ptr) {
        snprintf(buf, len, "N/A");
        return;
    }

    // Отображаем значение и текущий множитель
    uint8_t factor = factors[data->current_factor_idx];
    snprintf(buf, len, "%d (x%d)", *(data->value_ptr), factor);
}

const menu_node_strategy_t MENU_NUMERIC_STRATEGY = {
    .handle_push_button_fn      = numeric_push_button,
    .handle_long_push_button_fn = default_long_push_button,
    .handle_double_click_button_fn = numeric_double_click_button,
    .handle_delta_fn            = numeric_handle_delta,
    .title_fn                   = numeric_title_str,
    .value_fn                   = numeric_value_str
};