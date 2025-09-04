#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Объявляем узел меню как непрозрачный тип
typedef struct menu_node menu_node_t;

typedef void (*menu_action_callback_t)(int delta);
typedef void (*menu_print_callback_t)(char *value);

// Публичный интерфейс API меню
void menu_init(void);

menu_node_t * menu_activate_node(menu_node_t *parent, const char *title, menu_action_callback_t action, menu_print_callback_t print);
void menu_deactivate_node(menu_node_t *node);

const char * menu_title(void);
const char * menu_value(void);

void menu_set_current(menu_node_t *node);

const menu_node_t *menu_next_cycle(const menu_node_t *node);
const menu_node_t *menu_prev_cycle(const menu_node_t *node);
const menu_node_t *menu_next(const menu_node_t *node);
const menu_node_t *menu_prev(const menu_node_t *node);

bool menu_handle_delta(int delta);

bool menu_has_action(void);
bool menu_has_print(void);

void menu_enter(void);
void menu_out(void);

const menu_node_t * menu_get_root(void);
const menu_node_t * menu_get_first_child(menu_node_t *node);
const menu_node_t *menu_get_parent(menu_node_t *node);
const char * menu_node_title(menu_node_t *node);

#ifdef __cplusplus
}
#endif

#endif // MENU_H