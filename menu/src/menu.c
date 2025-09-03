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
    menu_node_t *items;
#endif
    uint8_t state;
} menu_context_t;

static menu_context_t s_context = { 0 };

static void s_menu_prepare_chain(menu_node_t *parent);
static menu_node_t* s_menu_activate_node(void);
static menu_node_t* s_menu_activate_current(void);

void menu_init(void)
{
    s_context.current = NULL;
#if MENU_USAGE_MEMORY == MENU_USAGE_DYNAMIC_MEMORY
    s_context.items = (menu_node_t *)malloc(sizeof(menu_node_t) * MENU_MAX_ITEMS);
#endif
    s_context.counter = 0;

    for(int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        menu_node_t *node = &(s_context.items[i]);
        memset(node, 0, sizeof(menu_node_t));
    }

    strncpy(s_context.root.title, "Main", MENU_TITLE_LEN);
    SET_FLAG(s_context.root.state, MENU_NODE_INITED);
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

#if 1
    if (parent) {
        if (!parent->first_child) {
            // первый ребёнок
            parent->first_child = node;
            parent->last_child = node;
        } else {
            // добавляем в конец
            parent->last_child->next = node;
            node->prev = parent->last_child;
            parent->last_child = node;
        }
    }
#endif        

    // s_menu_prepare_chain(parent);

    return node;
}

void menu_deactivate_node(menu_node_t *item)
{
    if (item == &(s_context.root))
        return;
    menu_node_t *parent = item->parent;
    memset(item, 0, sizeof(menu_node_t));
    s_menu_prepare_chain(parent);
}

menu_node_t * menu_navigate_delta(int16_t delta)
{
    menu_node_t *handle = 0;
    s_menu_activate_current();
    if (delta > 0)
    {
        for(int i = 0; i < delta; i++)
        {
            s_context.current = menu_next(s_context.current);
        }
    } else {
        for (int i = 0; i < -delta; i++)
        {
            s_context.current = menu_prev(s_context.current);
        }
    }
    return s_context.current;
}

void menu_set_current(menu_node_t *item)
{
    s_context.current = item;
}

bool menu_navigate_to_parent(void)
{
    s_menu_activate_current();

    if (!s_context.current)
        return false;

    menu_node_t *parent = s_context.current->parent;
    if (!parent || parent == &(s_context.root))
        return false;

    s_context.current = parent;
    return true;
}

bool menu_navigate_to_child(void)
{
    menu_node_t *child = s_context.current->first_child;
    if (!child)
        return false;

    s_context.current = child;

    return true;
}

bool menu_handle_action(int delta)
{
    menu_node_t *item = s_menu_activate_current();
    if (!(item && item->action))
        return false;

    item->action(delta);
    return true;
}

menu_node_t *menu_next(menu_node_t *node) {
    return node->next ? node->next : node->parent->first_child;
}

menu_node_t *menu_prev(menu_node_t *node) {
    return node->prev ? node->prev : node->parent->last_child;
}

const char * menu_get_current_title(void)
{
    s_menu_activate_current();
    if (s_context.current == 0)
        return NULL;
    return s_context.current->title;
}

const char * menu_get_next_title(void)
{
    s_menu_activate_current();
    if (s_context.current == 0)
        return NULL;
    menu_node_t *next = menu_next(s_context.current);
    if (next == NULL)
        return NULL;
    return next->title;
}

const char * menu_get_parent_title(void)
{
    s_menu_activate_current();
    if (s_context.current == 0)
        return NULL;
    menu_node_t *parent = s_context.current->parent;
    if (parent == 0)
        return NULL;
    return parent->title;
}

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

bool menu_print_value(char *title, char *value)
{
    menu_node_t *item = s_menu_activate_current();
    
    if (!item)
        return false;
    
    snprintf(title, MENU_TITLE_LEN, "%.12s>", item->title);
    
    if (!item->print)
        return false;

    item->print(value);

    return true;
}

/**
 * Статические внутренние функции
 */

static void s_menu_prepare_chain(menu_node_t *parent)
{
    menu_node_t *first = NULL;
    menu_node_t *last = NULL;

    // Сначала находим все подходящие элементы и связываем их
    for (int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        menu_node_t *node = &(s_context.items[i]);

        if (IS_FLAG_RESET(node->state, MENU_NODE_INITED) || node->parent != parent)
            continue;
        
        if (first == NULL) {
            first = node;
        } else {
            last->next = node;
            node->prev = last;
        }
        last = node;
    }

    // Делаем список циклическим если есть элементы
    if (first && last) {
        first->prev = last;
        last->next = first;
    }
}

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

/**
 * TODO: Отладочная функция. На STM32 использоваться не будет. Потом убрать!
 */
void menu_print_items(void)
{
    for(int i = 0; i < s_context.counter; i++)
    {
        menu_node_t *item = &(s_context.items[i]);
        printf("[%s] <= [%s] => [%s]\n"
            , (item->prev) ? item->prev->title : "NULL"
            , item->title
            , (item->next) ? item->next->title : "NULL"
        );
    }
}

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

const char * menu_title(void)
{
    menu_node_t *node = s_menu_activate_current();

    if (node == 0)
        return NULL;

    if (IS_FLAG_SET(node->state, MENU_NODE_ACTION))
    {
        // printf("%s:%d NODE INITED\n", __FILE__, __LINE__);
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
            s_context.current = menu_next(s_context.current);
        }
    } else {
        for (int i = 0; i < -delta; i++)
        {
            s_context.current = menu_prev(s_context.current);
        }
    }

    return true;
}