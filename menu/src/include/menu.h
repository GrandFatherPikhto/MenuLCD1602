#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

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

/// Состояния узла меню
typedef enum {
    MENU_NODE_UNUSED = 0,
    MENU_NODE_ACTIVE,
    MENU_NODE_IN_ACTION
} menu_node_state_t;

/// Коллбек для обработки действия (например, изменение значения)
typedef void (*menu_node_action_t)(int8_t delta);

/// Коллбек для печати значения в строку
typedef void (*menu_node_print_t)(char *buf, size_t len);

/// Opaque-тип: структура объявлена, но не раскрыта
typedef struct menu_node menu_node_t;

/// ---------------- API ----------------

void menu_init(void);

menu_node_t *menu_create_node(const char *title,
                              menu_node_action_t action,
                              menu_node_print_t print);

void menu_set_root(menu_node_t *node);
void menu_set_current(menu_node_t *node);

menu_node_t *menu_root(void);
menu_node_t *menu_current(void);

bool menu_is_root(const menu_node_t *node);
bool menu_is_leaf(const menu_node_t *node);
bool menu_is_action(const menu_node_t *node);

void menu_add_child(menu_node_t *parent, menu_node_t *child);

void menu_enter(void);
void menu_out(void);
void menu_next(void);
void menu_prev(void);

void menu_handle_action(void);
void menu_handle_delta(int8_t delta);

menu_node_t *menu_child(menu_node_t *node);
menu_node_t *menu_parent(menu_node_t *node);

const char *menu_title(menu_node_t *node);
const char *menu_value(menu_node_t *node);

#ifdef __cplusplus
}
#endif
