#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "menu.h"

#define ENCODER_INPUT_FILTER   2 ///< Значение фильтра Rotary Encode

#ifdef __cplusplus
extern "C" {
#endif

void rotary_encoder_init(menu_handle_t *handle);
void rotary_encoder_callback (int current);
void rotary_encoder_push_button_callback (void);
void rotary_encoder_long_push_button_callback (void);

#ifdef __cplusplus
}
#endif

#endif // ROTARY_ENCODER_H