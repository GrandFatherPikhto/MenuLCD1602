#include "rotary_encoder.h"
#include "common.h"
#include "menu.h"
#include "console.h"
#include "print.h"

typedef enum {
    STATE_MENU = 0x0,
    STATE_CALLBACK = 0x1,
} rotary_encoder_action_t;

typedef struct  {
    int current;
    int delta;
    int prev;
    rotary_encoder_action_t state;
} rotary_encoder_context_t;

static rotary_encoder_context_t s_context = {0};

static void s_reset_context(void);
static void s_display_menu(void);

void rotary_encoder_init(void)
{
    s_reset_context();
    s_context.state = STATE_MENU;
    menu_navigate_delta(0);
    s_display_menu();
    
    taskReadKey(rotary_encoder_callback, push_button_callback, long_push_button_callback);
}

void rotary_encoder_callback (int current)
{
    if ((current / ENCODER_INPUT_FILTER) * ENCODER_INPUT_FILTER != current) { // Дебаунсинг и Фильтрация
      return; // Неправильное значение, игнорировать
    }

    s_context.delta = (int)(current / ENCODER_INPUT_FILTER) - (int)s_context.current;
    s_context.prev  = s_context.current;
    s_context.current += s_context.delta;
    
    if (s_context.state == STATE_MENU)
    {
        menu_navigate_delta(s_context.delta);
    } else
    {
        menu_handle_action(s_context.delta);
    }

    s_display_menu();
}

void push_button_callback (void)
{
    if (s_context.state == STATE_MENU)
    {
        if (menu_has_action())
        {
            // s_reset_context();
            s_context.state = STATE_CALLBACK;
            menu_handle_action(0);
        } else {
            menu_navigate_to_child();            
        }
    } else if (s_context.state == STATE_CALLBACK)
    {
        s_context.state = STATE_MENU;
        menu_navigate_to_child();
    }

    s_display_menu();
}

/**
 * @brief Обратный вызов для обработки длительного нажатия кнопки, 
 * переходящий к родительскому элементу меню или к стартовому элементу меню.
 */
void long_push_button_callback (void)
{
    if (s_context.state == STATE_CALLBACK)
    {
        s_context.state = STATE_MENU;
    } else {
        menu_navigate_to_child();
    }

    s_display_menu();
}

static void s_reset_context(void)
{
    s_context.current = 0;
    s_context.delta = 0;
    s_context.prev = 0;
}

static void s_display_menu(void)
{
    if (s_context.state == STATE_MENU)
    {
        printMenu(menu_get_current_title(), menu_get_next_title());
    } else
    {
        char title[MENU_TITLE_LEN];
        char value[MENU_TITLE_LEN];
        menu_print_value(title, value);
        printMenu(title, value);
    }
}
