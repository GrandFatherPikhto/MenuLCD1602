#include "rotary_encoder.h"
#include "common.h"
#include "menu.h"
#include "lcd1602.h"
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

void rotary_encoder_init(void)
{
    s_reset_context();
    s_context.state = STATE_MENU;
    menu_navigate_delta(0);
}

void rotary_encoder_callback (int current)
{
    if ((current / ENCODER_INPUT_FILTER) * ENCODER_INPUT_FILTER != current) { // Дебаунсинг и Фильтрация
      return; // Неправильное значение, игнорировать
    }

    s_context.delta = (int)(current / ENCODER_INPUT_FILTER) - (int)s_context.current;
    s_context.prev  = s_context.current;
    s_context.current += s_context.delta;
    
    menu_handle_delta(s_context.delta);
#if 0
    if (s_context.state == STATE_MENU)
    {
        menu_navigate_delta(s_context.delta);
    } else
    {
        menu_handle_action(s_context.delta);
    }
#endif
}

void rotary_encoder_push_button_callback (void)
{
    menu_enter();
#if 0    
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
#endif        
}

/**
 * @brief Обратный вызов для обработки длительного нажатия кнопки, 
 * переходящий к родительскому элементу меню или к стартовому элементу меню.
 */
void rotary_encoder_long_push_button_callback (void)
{
    menu_out();
    /*
    if (s_context.state == STATE_CALLBACK)
    {
        s_context.state = STATE_MENU;
    } else {
        menu_navigate_to_child();
    }
    */
}

static void s_reset_context(void)
{
    s_context.current = 0;
    s_context.delta = 0;
    s_context.prev = 0;
}
