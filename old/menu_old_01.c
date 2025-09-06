#include "menu.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/// Полная структура спрятана здесь
struct menu_node {
    const char *title;

    const menu_node_strategy_t *strategy;

    struct menu_node *parent;
    struct menu_node *children;
    struct menu_node *children_last;
    struct menu_node *next;
    struct menu_node *prev;
    
    void *userdata; 

    uint8_t state; ///< menu_node_state_t
};

/// Контекст меню
typedef struct {
#if MENU_USAGE_STATIC_MEMORY
    menu_node_t items[MENU_MAX_ITEMS];
#endif
    menu_node_t *root;
    menu_node_t *current;
    char string[MENU_TITLE_LEN];
} menu_context_t;

static menu_context_t s_context;

/// --- внутренние функции ---
static void s_menu_prepare_chain(menu_node_t *parent);
static menu_node_t *s_menu_cycle(menu_node_t *node, bool forward);
static void s_menu_set_state(menu_node_t *node, menu_node_state_t state);

bool menu_is_root(const menu_node_t *node)
{
    return (node && node == s_context.root);
}

bool menu_is_leaf(const menu_node_t *node)
{
    return (node && !node->children);
}

bool menu_is_action(const menu_node_t *node)
{
    if (node == 0) return false;
    return node->state == MENU_NODE_IN_ACTION;
}

static void s_menu_set_state(menu_node_t *node, menu_node_state_t state)
{
    if (node) node->state = (uint8_t)state;
}

/// --- API реализация ---

void menu_init(void)
{
    memset(&s_context, 0, sizeof(s_context));
}

menu_node_t *menu_create_node(const char *title, menu_node_strategy_t *strategy)
{
#if MENU_USAGE_STATIC_MEMORY
    for (int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        if (s_context.items[i].state == MENU_NODE_UNUSED)
        {
            menu_node_t *node = &s_context.items[i];
            memset(node, 0, sizeof(*node));
            strncpy(node->title, MENU_TITLE_LEN, title);
            node->strategy;
            s_menu_set_state(node, MENU_NODE_ACTIVE);
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


void menu_set_root(menu_node_t *node)
{
    if (!node) return;

    s_context.root = node;
    s_context.current = node->children ? node->children : node;
}

void menu_set_current(menu_node_t *node)
{
    s_context.current = node;
}

menu_node_t *menu_root(void)
{
    return s_context.root;
}

menu_node_t *menu_current(void)
{
    return s_context.current;
}

void menu_add_child(menu_node_t *parent, menu_node_t *child)
{
    if (!parent || !child) return;
    
    if (child->parent) {
        // assert?
        return;
    }

    child->parent = parent;

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

void menu_enter(void)
{
    menu_node_t *cur = s_context.current;
    
    if (!cur) return;

    if (menu_is_root(cur))
        s_context.current = cur->children;

    if (cur->state == MENU_NODE_IN_ACTION && cur->push_button_fn)
    {
        cur->push_button_fn();
    } else if (!menu_is_leaf(cur))
    {
        s_context.current = cur->children;
    }
    else if (cur->action)
    {
        s_menu_set_state(cur, MENU_NODE_IN_ACTION);
    }
}

void menu_out(void)
{
    menu_node_t *node = s_context.current;
    if (!node || menu_is_root(node->parent)) return;

    if (node->state == MENU_NODE_IN_ACTION)
    {
        s_menu_set_state(node, MENU_NODE_ACTIVE);
    }
    else
    {
        s_context.current = node->parent;
    }
}

static menu_node_t *s_menu_cycle(menu_node_t *node, bool forward)
{
    if (!node) return NULL;
    return forward ? (node->next ? node->next : node->parent->children)
                   : (node->prev ? node->prev : node->parent->children_last);
}

void menu_next(void)
{
    s_context.current = s_menu_cycle(s_context.current, true);
}

const menu_node_t * menu_get_next(const menu_node_t *node)
{
    if (node == 0)
        return 0;
    return node->next;
}

const menu_node_t * menu_get_prev(const menu_node_t *node)
{
    if (node == 0)
        return 0;
    return node->prev;
}

void menu_prev(void)
{
    s_context.current = s_menu_cycle(s_context.current, false);
}

void menu_handle_action(void)
{
    menu_node_t *cur = s_context.current;
    if (!cur) return;

    if (cur->state == MENU_NODE_IN_ACTION && cur->action)
    {
        cur->action(1);
    }
    else
    {
        menu_enter();
    }
}

void menu_handle_delta(int8_t delta)
{
    menu_node_t *cur = s_context.current;
    if (!cur || delta == 0) return;

    if (cur->state == MENU_NODE_IN_ACTION && cur->action)
    {
        cur->action(delta);
    }
    else
    {
        s_context.current = s_menu_cycle(cur, delta > 0);
    }
}

const char *menu_title(const menu_node_t *node)
{
    return (node && node->title) ? node->title : "";   
}

const char *menu_value(const menu_node_t *node)
{
    static char buf[MENU_VALUE_LEN];
    if (!node) return "";

    if (node->print)
    {
        node->print(buf, sizeof(buf));
        return buf;
    }
    return "";
}

const char *menu_first_str(void)
{
    static char buf[MENU_TITLE_LEN];

    menu_node_t *node = s_context.current;
    if (node == 0)
        return "";

    if (node->state == MENU_NODE_IN_ACTION)
    {
        snprintf(buf, MENU_TITLE_LEN, "%s >", node->title);        
        return buf;
    } else if (node->state == MENU_NODE_ACTIVE)
    {
        snprintf(buf, MENU_TITLE_LEN, "%s >", 
            (node->parent && node->parent->title) ? node->parent->title : "");
        return buf;
    }

    return (node->parent && node->parent->title) ? node->parent->title : "";    
}

const char *menu_second_str(void)
{
    static char buf[MENU_VALUE_LEN];

    menu_node_t *node = s_context.current;

    if (node == 0)
        return "";
    
    if (node->state == MENU_NODE_IN_ACTION)
    {
        memset(buf, 0, MENU_VALUE_LEN);
        node->print(buf, sizeof(buf));
        return buf;
    }

    return (node && node->title) ? node->title : "";
}


const menu_node_t *menu_node_child(const menu_node_t *node)
{
    return node->children;
}

const menu_node_t *menu_node_child_last(const menu_node_t *node)
{
    return node->children_last;
}

const menu_node_t *menu_node_parent(const menu_node_t *node)
{
    return node->parent;
}
