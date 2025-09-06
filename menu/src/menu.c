#include "menu.h"

struct menu_node {
    char title[MENU_TITLE_LEN];

    struct menu_node *parent;
    struct menu_node *children;
    struct menu_node *children_last;
    struct menu_node *next;
    struct menu_node *prev;
    
    void *userdata; 

    const menu_node_strategy_t *strategy;

    menu_node_state_t state;
};

struct menu_handle {
#if MENU_USAGE_STATIC_MEMORY
    struct menu_node items[MENU_MAX_ITEMS];
#elif MENU_USAGE_DYNAMIC_MEMORY
    
#endif
    struct menu_node *root;
    struct menu_node *current;
};

static void s_menu_prepare_chain(menu_node_t *parent);
static void s_menu_prepare_root(menu_handle_t *handle);

void menu_init(menu_handle_t **handle)
{
    static menu_handle_t menu_handle = {0};
    *handle = &menu_handle;
    // printf("size %d\n", sizeof(menu_handle_t));
    s_menu_prepare_root(&menu_handle);
}

menu_node_t * menu_create_node(menu_handle_t *handle, const char *title, const menu_node_strategy_t *strategy)
{
#if MENU_USAGE_STATIC_MEMORY
    for (int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        if (handle->items[i].state == MENU_NODE_UNUSED)
        {
            menu_node_t *node = &(handle->items[i]);
            memset(node, 0, sizeof(menu_node_t));
            strncpy(node->title, title, MENU_TITLE_LEN);
            node->strategy = strategy;
            node->state = MENU_NODE_ACTIVE;
            menu_add_child(handle, handle->root, node);
            return node;
        }
    }
    return NULL;
#else
    menu_node_t *node = malloc(sizeof(menu_node_t));
    if (!node) return NULL;
    memset(node, 0, sizeof(*node));
    strncpy(node->title, MENU_TITLE_LEN, title);
    node->strategy = strategy;
    s_menu_set_state(node, MENU_NODE_ACTIVE);
    return node;
#endif
}

/**
 * TODO: добавь проверку, что нода уже есть!
 */
void menu_add_child(menu_handle_t *handle, menu_node_t *parent, menu_node_t *child)
{
    if (!parent || !child) return;
    
    if (parent == NULL)
    {
        child->parent = handle->root;
    } else {
        child->parent = parent;
    }

    if (!parent->children)
    {
        parent->children = parent->children_last = child;
    }
    else
    {
        parent->children_last->next = child;
        child->prev = parent->children_last;
        parent->children_last = child;
    }

    s_menu_prepare_chain(parent);
}

void menu_set_current(menu_handle_t *handle, menu_node_t *node)
{
    handle->current = (menu_node_t *)node;
}

void menu_node_set_userdata(menu_node_t *node, void *data)
{
    if (node == 0)
        return;
    node->userdata = data;
}

void menu_handle_push_button(menu_handle_t *handle, menu_node_t *node) {
    menu_node_t *current = node;
    if (current == 0)
        current = handle->current;

    if (current == 0)
        return;

    if (current && current->strategy && current->strategy->handle_push_button_fn) {
        current->strategy->handle_push_button_fn(handle, current);
    }
}

void menu_handle_long_push_button(menu_handle_t *handle, menu_node_t *node) {
    menu_node_t *current = node;

    if (current == 0)
        current = handle->current;

    if (current == 0)
        return;

    if (current && current->strategy && current->strategy->handle_long_push_button_fn) {
        current->strategy->handle_long_push_button_fn(handle, current);
    }
}

void menu_handle_delta(menu_handle_t *handle, menu_node_t *node, int8_t delta) {
    menu_node_t *current = node;
    if (current == 0)
        current = handle->current;

    if (current == 0)
        return;


    if (current && current->strategy && current->strategy->handle_delta_fn) {
        current->strategy->handle_delta_fn(handle, current, delta);
    }
}

const char *menu_node_title(menu_node_t *node)
{
    if (node == 0)
        return "";
    return node->title;
}

void *menu_node_userdata(menu_node_t *node)
{
    if (node == 0)
        return 0;
    return node->userdata;
}

void menu_title_str(menu_handle_t *handle, menu_node_t *node, char *buf, size_t size) {
    menu_node_t *current = node;

    if (current == 0)
        current = menu_current(handle);

    if (current && current->strategy && current->strategy->title_fn) {
        memset(buf, 0, size);
        current->strategy->title_fn(handle, current, buf, size);
    }
}

void menu_value_str(menu_handle_t *handle, menu_node_t *node, char *buf, size_t size)
{
    menu_node_t *current = node;

    if (current == 0)
        current = menu_current(handle);

    if (current && current->strategy && current->strategy->title_fn) {
        memset(buf, 0, size);
        current->strategy->value_fn(handle, node, buf, size);
    }
}

void menu_next(menu_handle_t *handle, menu_node_t *node)
{
    menu_node_t *current = node;
    if (current == 0)
        current = menu_current(handle);

    if (current == 0)
        return;

    handle->current = menu_cycle(current, true);
}

void menu_prev(menu_handle_t *handle, menu_node_t *node)
{
    menu_node_t *current = node;
    if (current == 0)
        current = menu_current(handle);

    if (current == 0)
        return;

    handle->current = menu_cycle(current, false);
}

menu_node_t * menu_current(menu_handle_t *handle)
{
    if (handle->current == 0 && handle->root->children)
        handle->current = handle->root->children;

    return handle->current;
}

menu_node_t *menu_node_child(menu_node_t *node)
{
    return node->children;
}

menu_node_t *menu_node_child_last(menu_node_t *node)
{
    return node->children_last;
}

menu_node_t *menu_node_parent(menu_node_t *node)
{
    return node->parent;
}

menu_node_t *menu_resolve_current(menu_handle_t *handle, menu_node_t *node)
{
    if (!handle) return 0;

    if (!node)
        node = menu_current(handle);

    return node;
}

bool menu_is_root(menu_handle_t *handle, menu_node_t *node)
{
    return (node && node == handle->root);
}

bool menu_is_leaf(menu_node_t *node)
{
    return (node && !node->children);
}

menu_node_state_t menu_state(menu_node_t *node)
{
    if (node == 0) return false;
    return node->state;
}

menu_node_t *menu_cycle(menu_node_t *node, bool forward)
{
    if (!node) return 0;
    return forward ? (node->next ? node->next : node->parent->children)
                   : (node->prev ? node->prev : node->parent->children_last);
}

static void s_menu_prepare_chain(menu_node_t *parent)
{
    if (!parent || !parent->children) return;

    menu_node_t *first = parent->children;
    menu_node_t *last  = parent->children_last;

    if (first && last && first != last)
    {
        first->prev = last;
        last->next  = first;
    }
}

static void s_menu_prepare_root(menu_handle_t *handle)
{
    handle->root = &(handle->items[0]);
    handle->root->state = MENU_NODE_ACTIVE;
    strncpy(handle->root->title, "Root", MENU_TITLE_LEN);
}