#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#define ENCODER_INPUT_FILTER   2 ///< Значение фильтра Rotary Encode

#define ROTENC_FLAG_GOTO_PARENT 0x80
#define ROTENC_FLAG_EDIT_DATA   0x40
#define ROTENC_FLAG_GOTO_CHILD  0x20
#define ROTENC_FLAG_GOTO_CBFUNC 0x10

#ifdef __cplusplus
extern "C" {
#endif

void rotary_encoder_init(void);
void rotary_encoder_callback (int current);
void push_button_callback (void);
void long_push_button_callback (void);

#ifdef __cplusplus
}
#endif

#endif // ROTARY_ENCODER_H