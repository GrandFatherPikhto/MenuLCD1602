#include "strategy_default.h"

void default_push_button(menu_handle_t *h, menu_node_t *n) { (void)h; (void)n; }
void default_long_push_button(menu_handle_t *h, menu_node_t *n) {
    menu_node_t *current = menu_resolve_current(h, n);
    if (!current) return;
    menu_node_t *parent = menu_node_parent(current);
    if (parent && !menu_is_root(h, parent)) {
        menu_set_current(h, parent); // по умолчанию "выход вверх"
    }
}

void default_double_click_button(menu_handle_t *h, menu_node_t *n) {
    (void)h; (void)n;
}

void default_handle_delta(menu_handle_t *h, menu_node_t *n, int8_t d) { (void)h; (void)n; (void)d; }
void default_title_str(menu_handle_t *h, const menu_node_t *n, char *b, size_t l) {
    (void)h;
    snprintf(b, l, "%s", menu_node_title((menu_node_t*)n));
}
void default_value_str(menu_handle_t *h, const menu_node_t *n, char *b, size_t l) {
    (void)h; (void)n;
    if (l > 0) b[0] = '\0';
}

