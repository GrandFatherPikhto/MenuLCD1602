#include "menu.h"
#include "strategy_default.h"
#include "strategy_submenu.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Обработка короткого нажатия: спускаемся на первый дочерний узел
 */
static void submenu_push_button(menu_handle_t *handle, menu_node_t *node) {
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    if (menu_node_child(current)) {
        menu_set_current(handle, menu_node_child(current));
    }
}


/**
 * @brief Обработка вращения энкодера: переход к следующему/предыдущему узлу
 */
static void submenu_handle_delta(menu_handle_t *handle, menu_node_t *node, int8_t delta) {
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    menu_node_t *next = menu_cycle(current, delta > 0);
    if (next) {
        menu_set_current(handle, next);
    }
}

/**
 * @brief Формируем строку заголовка для LCD (отображает родителя)
 */
static void submenu_title_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    const menu_node_t *current = menu_resolve_current(handle, (menu_node_t *)node);
    if (!current) return;

    menu_node_t *parent = menu_node_parent((menu_node_t *)current);
    if (!parent) return;

    snprintf(buf, len, "%.12s >", menu_node_title(parent));
}

/**
 * @brief Формируем строку значения для LCD (отображает сам узел)
 */
static void submenu_value_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    menu_node_t *current = menu_resolve_current(handle, (menu_node_t *)node);
    if (!current) return;

    strncpy(buf, menu_node_title(current), len - 1);
    buf[len - 1] = '\0';
}

/**
 * @brief Экспортируемая стратегия
 */
const menu_node_strategy_t MENU_SUBMENU_STRATEGY = {
    .handle_push_button_fn      = submenu_push_button,
    .handle_long_push_button_fn = &default_long_push_button,
    .handle_delta_fn            = submenu_handle_delta,
    .title_fn                   = submenu_title_str,
    .value_fn                   = submenu_value_str
};
