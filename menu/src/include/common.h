#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>

#define MENU_TITLE_LEN 0x20
#define MENU_MAX_ITEMS 0x20

#define CONFIG_PWM_FREQUENCY 10000000 ///< Частота ШИМ

#define CONFIG_PWM_DIVIDER_MAX 10000  ///< Максимальное значение делителя ШИМ
#define CONFIG_PWM_DIVIDER_MIN 2      ///< Минимальное значение делителя ШИМ
#define CONFIG_CONFIG_PWM_FREQUENCY_MAX 1000000 ///< Максимальное значение частоты ШИМ
#define CONFIG_CONFIG_PWM_FREQUENCY_MIN 5000    ///< Минимальное значение частоты ШИМ

#define IS_FLAG_SET(flags, flag) (((flags) & (flag)) == (flag))
#define IS_FLAG_RESET(flags, flag) (((flags) & (flag)) == 0)
#define SET_FLAG(flags, flag) ((flags) |= (flag))
#define RESET_FLAG(flags, flag) ((flags) &= ~(flag))
#define TOGGLE_FLAG(flags, flag) ((flags) ^= (flag))

#endif // COMMON_H