#ifndef LCD1602_H
#define LCD1602_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* rotary_encoder_callback_t) (int current);
typedef void (* push_button_callback_t) (void);
typedef void (* long_push_buttont_callback_t) (void);

typedef struct lcd1602_handle lcd1602_handle_t;

void lcd_clear(lcd1602_handle_t *lcd);
void lcd_set_cursor(lcd1602_handle_t *lcd, int x, int y);
void lcd_print_char(lcd1602_handle_t *lcd, char ch);
void lcd_print_str(lcd1602_handle_t *lcd, const char *str);

bool lcd1602_init(rotary_encoder_callback_t rotary_encoder_callback_func, push_button_callback_t push_button_callback_func, long_push_buttont_callback_t long_push_button_callback_func);

#ifdef __cplusplus
}
#endif

#endif // LCD1602_H
