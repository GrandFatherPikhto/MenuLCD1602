#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <config.h>

#include "menu.h"
#include "common.h"

#define MENU_USAGE_STATIC_MEMORY 1
#define MENU_USAGE_DYNAMIC_MEMORY 2
#define MENU_USAGE_MEMORY MENU_USAGE_STATIC_MEMORY

#define MENU_NODE_INITED 0x01
#define MENU_NODE_ACTION 0x02

typedef struct menu_node {
    char title[MENU_TITLE_LEN];
    char value[MENU_TITLE_LEN];

    struct menu_node *prev;
    struct menu_node *next;
    struct menu_node *parent;
    struct menu_node *first_child;
    struct menu_node *last_child;

    menu_action_callback_t action;
    menu_print_callback_t print;

    uint8_t state;
} menu_node_t;

typedef struct {
    struct menu_node root;
    menu_node_t *current;
#if MENU_USAGE_MEMORY == MENU_USAGE_STATIC_MEMORY
    menu_node_t items[MENU_MAX_ITEMS];
    uint16_t counter;
#else
    
#endif
    uint8_t state;
} menu_context_t;

static menu_context_t s_context = { 0 };

static menu_node_t* s_menu_activate_node(void);
static menu_node_t* s_menu_activate_current(void);
static void s_menu_activate_root(void);
menu_node_t *s_menu_next_cycle (const menu_node_t *node);
menu_node_t *s_menu_prev_cycle (const menu_node_t *node);

void menu_init(void)
{
    s_context.current = NULL;
#if MENU_USAGE_MEMORY == MENU_USAGE_DYNAMIC_MEMORY
    
#endif
    s_context.counter = 0;

    for(int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        menu_node_t *node = &(s_context.items[i]);
        memset(node, 0, sizeof(menu_node_t));
    }

    s_menu_activate_root();
}

menu_node_t * menu_activate_node(menu_node_t *parent, const char *title, menu_action_callback_t action, menu_print_callback_t print)
{
    if (parent == 0)
        parent = &(s_context.root);

    menu_node_t *node = s_menu_activate_node();

    if (!node)
    {
        return NULL;
    }

    node->parent = parent;
    strncpy(node->title, title, MENU_TITLE_LEN);
    node->action = action;
    node->print = print;

    if (parent) {
        if (!parent->first_child) {
            parent->first_child = node;
            parent->last_child = node;
        } else {
            parent->last_child->next = node;
            node->prev = parent->last_child;
            parent->last_child = node;
        }
    }

    return node;
}

void menu_deactivate_node(menu_node_t *item)
{
    if (item == &(s_context.root))
        return;
    menu_node_t *parent = item->parent;
    memset(item, 0, sizeof(menu_node_t));
}

menu_node_t * menu_navigate_delta(int16_t delta)
{
    menu_node_t *handle = 0;
    s_menu_activate_current();
    if (delta > 0)
    {
        for(int i = 0; i < delta; i++)
        {
            s_context.current = s_menu_next_cycle(s_context.current);
        }
    } else {
        for (int i = 0; i < -delta; i++)
        {
            s_context.current = s_menu_prev_cycle(s_context.current);
        }
    }
    return s_context.current;
}

void menu_set_current(menu_node_t *item)
{
    s_context.current = item;
}

bool menu_handle_action(int delta)
{
    menu_node_t *item = s_menu_activate_current();
    if (!(item && item->action))
        return false;

    item->action(delta);
    return true;
}

/**
 * Функции перемещения по меню
 */

/**
 * @brief
 */
void menu_enter(void)
{
    menu_node_t *node = s_menu_activate_current();
    if (node == 0)
        return;

    if (node->action) {
        TOGGLE_FLAG(node->state, MENU_NODE_ACTION);
    } else if (node->first_child)
    {
        s_context.current = s_context.current->first_child;
    }
}

void menu_out(void)
{
    menu_node_t *node = s_menu_activate_current();
    if (node == 0)
        return;

    if (IS_FLAG_SET(node->state, MENU_NODE_ACTION))
    {
        RESET_FLAG(node->state, MENU_NODE_ACTION);
    } else if (node->parent != &(s_context.root))
    {
        s_context.current = &(s_context.root);
    }
}

const menu_node_t *menu_next_cycle(const menu_node_t *node) {
    if (node == 0)
        return 0;
    return node->next ? node->next : node->parent->first_child;
}

const menu_node_t *menu_prev_cycle(const menu_node_t *node) {
    if (node == 0)
        return 0;
    return node->prev ? node->prev : node->parent->last_child;
}

const menu_node_t *menu_next(const menu_node_t *node) {
    if (node == 0)
        return 0;
    return node->next;
}

const menu_node_t *menu_prev(const menu_node_t *node) {
    if (node == 0)
        return 0;
    return node->prev;
}

bool menu_handle_delta(int delta)
{
    if (s_context.current == 0)
        return false;

    menu_node_t *node = s_context.current;

    if (IS_FLAG_SET(s_context.current->state, MENU_NODE_ACTION) && node->action)
    {
        node->action(delta);
        return true;
    }

    if (delta > 0)
    {
        for(int i = 0; i < delta; i++)
        {
            s_context.current = s_menu_next_cycle(s_context.current);
        }
    } else {
        for (int i = 0; i < -delta; i++)
        {
            s_context.current = s_menu_prev_cycle(s_context.current);
        }
    }

    return true;
}


/**
 * Функции доступа ко внутренним членам структуры
 */

bool menu_has_action(void)
{
    s_menu_activate_current();
    return s_context.current->action != NULL;
}

bool menu_has_print(void)
{
    s_menu_activate_current();
    return s_context.current->print != NULL;
}

const char * menu_title(void)
{
    menu_node_t *node = s_menu_activate_current();

    if (node == 0)
        return NULL;

    if (IS_FLAG_SET(node->state, MENU_NODE_ACTION))
    {
        return node->title;
    } 

    if (node->parent == 0)
        return NULL;
    
    return node->parent->title;
}

const char * menu_value(void)
{
    if (s_context.current == 0)
        return NULL;
    menu_node_t *node = s_context.current;

    if (IS_FLAG_SET(s_context.current->state, MENU_NODE_ACTION) && node->print)
    {
        memset(node->value, 0, MENU_TITLE_LEN);
        node->print(node->value);
        return node->value;
    }

    return node->title;
}

const menu_node_t * menu_get_root(void)
{
    return &(s_context.root);
}

const menu_node_t * menu_get_first_child(menu_node_t *node)
{
    if (node == 0)
        return 0;

    return node->first_child;
}

const menu_node_t *menu_get_parent(menu_node_t *node)
{
    if (node == 0)
        return 0;

    return node->parent;
}

const char * menu_node_title(menu_node_t *node)
{
    if (node == 0)
        return 0;

    return node->title;
}

/**
 * Статические внутренние функции
 */


static menu_node_t* s_menu_activate_node(void)
{
    menu_node_t *node = NULL;
#if MENU_USAGE_MEMORY == MENU_USAGE_DYNAMIC_MEMORY
    return (menu_node_t *) malloc(sizeof(menu_node_t));
#else
    for (int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        node = &(s_context.items[i]);
        if (IS_FLAG_SET(node->state, MENU_NODE_INITED))
            continue;

        memset(node, 0, sizeof(menu_node_t));
        SET_FLAG(node->state, MENU_NODE_INITED);
        s_context.counter ++;

        return node;
    }

    return NULL;
#endif
}

static menu_node_t* s_menu_activate_current()
{
    if (s_context.current == NULL)
    {
        s_context.current = s_context.root.first_child;
    }

    return s_context.current;
}

static void s_menu_activate_root(void)
{
    memset(&s_context.root, 0, sizeof(menu_node_t));
    strncpy(s_context.root.title, "Main", MENU_TITLE_LEN);
    SET_FLAG(s_context.root.state, MENU_NODE_INITED);
}

menu_node_t *s_menu_next_cycle (const menu_node_t *node) {
    if (node == 0)
        return 0;
    return node->next ? node->next : node->parent->first_child;
}

menu_node_t *s_menu_prev_cycle (const menu_node_t *node) {
    if (node == 0)
        return 0;
    return node->prev ? node->prev : node->parent->last_child;
}