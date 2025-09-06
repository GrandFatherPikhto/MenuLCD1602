#include "strategy_toggle.h"
#include "strategy_default.h"
#include <stdio.h>
#include <string.h>

static void toggle_push_button(menu_handle_t *handle, menu_node_t *node) {
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;
    
    toggle_strategy_data_t *data = (toggle_strategy_data_t *)menu_node_userdata(current);
    
    if (data) {
        TOGGLE_FLAG(data->flags, TOGGLE_STRATEGY_FLAG_ENABLE);
        bool enable = IS_FLAG_SET(data->flags, TOGGLE_STRATEGY_FLAG_ENABLE);
        printf("[%s] = %s\n", menu_node_title(current), enable ? "ON" : "OFF");
        if (data->change_cb)
        {
            data->change_cb(enable);
        }
    }
}

static void toggle_handle_delta(menu_handle_t *handle, menu_node_t *node, int8_t delta) {
    (void)handle; (void)node; (void)delta;
    // игнорируем вращение
}

static void toggle_handle_long_push_button(menu_handle_t *handle, menu_node_t *node) {
    menu_node_t *current = menu_resolve_current(handle, node);
    if (!current) return;

    toggle_strategy_data_t *data = (toggle_strategy_data_t *)menu_node_userdata(current);
    if (IS_FLAG_SET(data->flags, TOGGLE_STRATEGY_FLAG_DISABLE_ON_EXIT) && IS_FLAG_SET(data->flags, TOGGLE_STRATEGY_FLAG_ENABLE))
    {
        RESET_FLAG(data->flags, TOGGLE_STRATEGY_FLAG_ENABLE);
        if (data->change_cb)
        {
            data->change_cb(false);
        }
    }

    default_long_push_button(handle, node);
}

static void toggle_title_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    (void)handle;
    snprintf(buf, len, "%s", menu_node_title((menu_node_t *)node));
}

static void toggle_value_str(menu_handle_t *handle, const menu_node_t *node, char *buf, size_t len) {
    menu_node_t *current = menu_resolve_current(handle, (menu_node_t*)node);
    const toggle_strategy_data_t *data = (toggle_strategy_data_t *)menu_node_userdata(current);
    bool enable = false;
    if (data != 0)
    {
        enable = IS_FLAG_SET(data->flags, TOGGLE_STRATEGY_FLAG_ENABLE);
    }
    printf("%s:%d %s %b\n", __FILE__, __LINE__, menu_node_title(current), enable);
    snprintf(buf, len, "%s", enable ? "ON" : "OFF");
}

const menu_node_strategy_t MENU_TOGGLE_STRATEGY = {
    .handle_push_button_fn      = toggle_push_button,
    .handle_long_push_button_fn = toggle_handle_long_push_button,
    .handle_double_click_button_fn = default_double_click_button,
    .handle_delta_fn            = toggle_handle_delta,
    .title_fn                   = toggle_title_str,
    .value_fn                   = toggle_value_str
};
