#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_CLEAR

typedef void (* rotary_encoder_callback_t) (int current);
typedef void (* push_button_callback_t) (void);
typedef void (* long_push_buttont_callback_t) (void);

void printMenu(const char *str1, const char *str2);
void taskReadKey(rotary_encoder_callback_t rotary_encoder_callback_func, push_button_callback_t push_button_callback_func, long_push_buttont_callback_t long_push_button_callback_func);

#ifdef __cplusplus
}
#endif

#endif // CONSOLE_H
