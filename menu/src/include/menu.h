#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"

#ifndef MENU_USAGE_STATIC_MEMORY
#define MENU_USAGE_STATIC_MEMORY 1
#endif

#ifndef MENU_MAX_ITEMS
#define MENU_MAX_ITEMS 32
#endif

#ifndef MENU_TITLE_LEN
#define MENU_TITLE_LEN 0x20
#endif

#ifndef MENU_VALUE_LEN
#define MENU_VALUE_LEN 0x20
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MENU_NODE_UNUSED = 0,
    MENU_NODE_ACTIVE,
    MENU_NODE_IN_ACTION
} menu_node_state_t;

/// Opaque-тип: структура объявлена, но не раскрыта
typedef struct menu_node menu_node_t;
typedef struct menu_handle menu_handle_t;

typedef void (*menu_node_push_button_t)(menu_handle_t *, menu_node_t *node);
typedef void (*menu_node_long_push_button_t)(menu_handle_t *, menu_node_t *);
typedef void (*menu_node_double_click_button_t)(menu_handle_t *, menu_node_t *);
typedef void (*menu_node_delta_t)(menu_handle_t *, menu_node_t *, int8_t);
typedef void (*menu_node_title_t)(menu_handle_t *, const menu_node_t *, char *, size_t);
typedef void (*menu_node_value_t)(menu_handle_t *, const menu_node_t *, char *, size_t);
typedef void (*menu_node_change_value_t)(void *data);

typedef struct menu_node_strategy {
    menu_node_push_button_t handle_push_button_fn;
    menu_node_long_push_button_t handle_long_push_button_fn;
    menu_node_double_click_button_t handle_double_click_button_fn;
    menu_node_delta_t handle_delta_fn;
    menu_node_title_t title_fn;
    menu_node_value_t value_fn;
} menu_node_strategy_t;

/// ---------------- API ----------------

void menu_init(menu_handle_t **handle);

menu_node_t *menu_create_node(menu_handle_t *handle, const char *title, const menu_node_strategy_t *strategy);
void menu_add_child(menu_handle_t *handle, menu_node_t *parent, menu_node_t *child);
void menu_set_current(menu_handle_t *handle, menu_node_t *node);

bool menu_is_root(menu_handle_t *handle, menu_node_t *node);
bool menu_is_leaf(menu_node_t *node);
menu_node_state_t menu_state(menu_node_t *node);

menu_node_t *menu_current(menu_handle_t *handle);
menu_node_t *menu_node_child(menu_node_t *node);
menu_node_t *menu_node_child_last(menu_node_t *node);
menu_node_t *menu_node_parent(menu_node_t *node);
menu_node_t *menu_resolve_current(menu_handle_t *handle, menu_node_t *node);

void menu_handle_push_button(menu_handle_t *handle, menu_node_t *node);
void menu_handle_long_push_button(menu_handle_t *handle, menu_node_t *node);
void menu_handle_delta(menu_handle_t *handle, menu_node_t *node, int8_t delta);

const char * menu_node_title(menu_node_t *node);
void * menu_node_userdata(menu_node_t *node);

void menu_node_set_userdata(menu_node_t *node, void *data);

void menu_node_set_change_value_cb(menu_node_t *node, menu_node_change_value_t change_value_cb);
menu_node_change_value_t menu_node_change_value_cb(menu_node_t *node);

void menu_next(menu_handle_t *handle, menu_node_t *node);
void menu_prev(menu_handle_t *handle, menu_node_t *node);

void menu_title_str(menu_handle_t *handle, menu_node_t *node, char *buf, size_t size);
void menu_value_str(menu_handle_t *handle, menu_node_t *node, char *buf, size_t size);

menu_node_t *menu_cycle(menu_node_t *node, bool forward);

#ifdef __cplusplus
}
#endif
