#include "rotary_encoder.h"
#include "common.h"
#include "menu.h"
#include "lcd1602.h"
#include "print.h"

typedef struct  {
    int current;
    int delta;
    int prev;
    menu_handle_t *menu_handle;    
} rotary_encoder_context_t;

static rotary_encoder_context_t s_context = {0};

static void s_reset_context(void);

static menu_handle_t *s_menu_handle;

void rotary_encoder_init(menu_handle_t *handle)
{
    s_reset_context();
    s_context.menu_handle = handle;
}

void rotary_encoder_callback (int current)
{
    if ((current / ENCODER_INPUT_FILTER) * ENCODER_INPUT_FILTER != current) { // Дебаунсинг и Фильтрация
      return; // Неправильное значение, игнорировать
    }

    s_context.delta = (int)(current / ENCODER_INPUT_FILTER) - (int)s_context.current;
    s_context.prev  = s_context.current;
    s_context.current += s_context.delta;
    
    menu_handle_delta(s_context.menu_handle, 0, s_context.delta);
}

void rotary_encoder_push_button_callback (void)
{
    menu_handle_push_button(s_context.menu_handle, 0);
}

/**
 * @brief Обратный вызов для обработки длительного нажатия кнопки, 
 * переходящий к родительскому элементу меню или к стартовому элементу меню.
 */
void rotary_encoder_long_push_button_callback (void)
{
    menu_handle_long_push_button(s_context.menu_handle, 0);
}

static void s_reset_context(void)
{
    s_context.current = 0;
    s_context.delta = 0;
    s_context.prev = 0;
    s_context.menu_handle = 0;
}
