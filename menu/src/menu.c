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

#define MENU_ITEM_INITED 0x01

typedef struct menu_node {
    char title[MENU_TITLE_LEN];
    struct menu_node *prev;
    struct menu_node *next;
    struct menu_node *parent;
    menu_action_callback_t action;   //< Функция обратного вызова, выполняемая при взаимодействии с элементом
    menu_print_callback_t print;
    uint8_t state;
} menu_node_t;

typedef struct {
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
static menu_node_t* s_menu_get_current_parent(void);
static menu_node_t* s_menu_get_current_child(void);
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
        menu_node_t *item = &(s_context.items[i]);
        memset(item, 0, sizeof(menu_node_t));
    }
}

menu_node_t * menu_activate_node(menu_node_t *parent, const char *title, menu_action_callback_t action, menu_print_callback_t print)
{
    menu_node_t *item = s_menu_activate_node();

    item->parent = parent;
    strncpy(item->title, title, MENU_TITLE_LEN);
    item->action = action;
    item->print = print;
    s_context.counter ++;

    s_menu_prepare_chain(parent);

    return item;
}

void menu_deactivate_node(menu_node_t *item)
{
    menu_node_t *parent = item->parent;
    RESET_FLAG(item->state, MENU_ITEM_INITED);
    menu_reset_node(item);
    s_menu_prepare_chain(parent);
}

void menu_reset_node(menu_node_t *item)
{
    item->parent = NULL;
    item->action = NULL;
    item->next = NULL;
    item->prev = NULL;
    item->state = 0;
    memset(item->title, 0, MENU_TITLE_LEN);
}

menu_node_t * menu_navigate_delta(int16_t delta)
{
    menu_node_t *handle = 0;
    s_menu_activate_current();
    if (delta > 0)
    {
        for(int i = 0; i < delta; i++)
        {
            s_context.current = s_context.current->next;
        }
    } else {
        for (int i = 0; i < -delta; i++)
        {
            s_context.current = s_context.current->prev;
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
    menu_node_t *parent = s_menu_get_current_parent();
    if (!parent)
        return false;

    s_context.current = parent;
    return true;
}

bool menu_navigate_to_child(void)
{
    menu_node_t *child = s_menu_get_current_child();
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

const char * menu_get_current_title(void)
{
    menu_node_t *item = s_menu_activate_current();
    if (!item)
        return NULL;
    return item->title;
}

const char * menu_get_next_title(void)
{
    menu_node_t *item = s_menu_activate_current();
    if (!item || !item->next)
        return NULL;
    return item->next->title;
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
    menu_node_t *item = NULL, *prev = NULL, *first = NULL;
    for (int i = 0; i < s_context.counter; i++)
    {
        item = &(s_context.items[i]);

        if (item->parent != parent)
            continue;

        if (!first)
            first = item;

        item->prev = prev;

        if (prev)
            prev->next = item;

        prev = item;
    }

    first->prev = item;
    item->next = first;
}

static menu_node_t* s_menu_activate_node(void)
{
    menu_node_t *item = NULL;
#if MENU_USAGE_MEMORY == MENU_USAGE_DYNAMIC_MEMORY
    return (menu_node_t *) malloc(sizeof(menu_node_t));
#else
    for (int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        if (IS_FLAG_RESET(s_context.items[i].state, MENU_ITEM_INITED))
        {
            SET_FLAG(s_context.items[i].state, MENU_ITEM_INITED);
            return &(s_context.items[i]);
        }
    }
#endif
}

menu_node_t * s_menu_get_current_parent(void)
{
    menu_node_t *current = s_menu_activate_current();
    if (current)
        return current->parent;
    
    return NULL;
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

static menu_node_t* s_menu_get_current_child(void)
{
    menu_node_t *parent = s_menu_activate_current();
    for (int i = 0; i < MENU_MAX_ITEMS; i++)
    {
        menu_node_t *item = &(s_context.items[i]);
        if (item->parent == parent)
            return item;
    }

    return NULL;
}

menu_node_t* s_menu_activate_current()
{
    if (s_context.current == NULL)
    {
        for (int i = 0; i < MENU_MAX_ITEMS; i++)
        {
            menu_node_t * item = &(s_context.items[i]);
            if (item->parent == NULL)
            {
                s_context.current = item;
                break;
            }
        }
    }

    return s_context.current;
}
