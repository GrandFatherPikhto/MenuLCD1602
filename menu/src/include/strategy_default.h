#ifndef MENU_DEFAULT_STRATEGY_H
#define MENU_DEFAULT_STRATEGY_H

#include "menu.h"
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void default_push_button(menu_handle_t *h, menu_node_t *n);
void default_long_push_button(menu_handle_t *h, menu_node_t *n);
void default_double_click_button(menu_handle_t *h, menu_node_t *n);

void default_handle_delta(menu_handle_t *h, menu_node_t *n, int8_t d);
void default_title_str(menu_handle_t *h, const menu_node_t *n, char *b, size_t l);
void default_value_str(menu_handle_t *h, const menu_node_t *n, char *b, size_t l);

#ifdef __cplusplus
}
#endif

#endif // MENU_DEFAULT_STRATEGY_H