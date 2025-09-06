#include "strategy_action.h"
#include <stdio.h>
#include <string.h>

static void action_push_button(menu_handle_t *handle, menu_node_t *node) {
    (void)handle;
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    action_fn_t fn = (action_fn_t)menu_node_userdata(current);
    if (fn) fn(0); // событие "нажатие"
}

static void action_long_push_button(menu_handle_t *handle, menu_node_t *node) {
    (void)handle; (void)node;
    // по умолчанию ничего
}

static void action_handle_delta(menu_handle_t *handle, menu_node_t *node, int8_t delta) {
    (void)handle;
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    action_fn_t fn = (action_fn_t)menu_node_userdata(current);
    if (fn) fn(delta);
}

static void action_title_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    (void)handle;
    snprintf(buf, len, "%s", menu_node_title((menu_node_t *)node));
}

static void action_value_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    (void)handle; (void)node;
    // по умолчанию пустая строка
    if (len > 0) buf[0] = '\0';
}

const menu_node_strategy_t MENU_ACTION_STRATEGY = {
    .handle_push_button_fn      = action_push_button,
    .handle_long_push_button_fn = action_long_push_button,
    .handle_delta_fn            = action_handle_delta,
    .title_fn                   = action_title_str,
    .value_fn                   = action_value_str
};
