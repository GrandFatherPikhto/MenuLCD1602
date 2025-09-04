#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>

#define MENU_TITLE_LEN 0x20
#define MENU_VALUE_LEN 0x20
#define MENU_MAX_ITEMS 0x20

#define CONFIG_PWM_FREQUENCY 10000000 ///< Частота ШИМ 10 MHz

#define IS_FLAG_SET(flags, flag) (((flags) & (flag)) == (flag))
#define IS_FLAG_RESET(flags, flag) (((flags) & (flag)) == 0)
#define SET_FLAG(flags, flag) ((flags) |= (flag))
#define RESET_FLAG(flags, flag) ((flags) &= ~(flag))
#define TOGGLE_FLAG(flags, flag) ((flags) ^= (flag))

#endif // COMMON_H