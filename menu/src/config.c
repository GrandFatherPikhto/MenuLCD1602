#include <string.h>
#include <stdio.h>

#include "common.h"
#include "config.h"
#include "lcd1602.h"
#include "menu.h"

#define CONFIG_PWM_DIVIDER_MAX 10000  ///< Максимальное значение делителя ШИМ
#define CONFIG_PWM_DIVIDER_MIN 2      ///< Минимальное значение делителя ШИМ
#define CONFIG_CONFIG_PWM_FREQUENCY_MAX 1000000 ///< Максимальное значение частоты ШИМ
#define CONFIG_CONFIG_PWM_FREQUENCY_MIN 5000    ///< Минимальное значение частоты ШИМ

typedef struct {
    int divider;
    int frequency;
} frequency_t;

static const frequency_t s_frequencies[] = {
    {5, 2000000}, // 2.0 MHz
    {8, 1250000}, // 1.25 MHz
    {10, 1000000}, // 1.0 MHz
    {16, 625000}, // 0.625 MHz
    {20, 500000}, // 0.5 MHz
    {25, 400000}, // 0.4 MHz
    {32, 312500}, // 0.3125 MHz
    {40, 250000}, // 0.25 MHz
    {50, 200000}, // 0.2 MHz
    {64, 156250}, // 0.15625 MHz
    {80, 125000}, // 0.125 MHz
    {100, 100000}, // 0.1 MHz
    {125, 80000}, // 80.0 kHz
    {128, 78125}, // 78.125 kHz
    {160, 62500}, // 62.5 kHz
    {200, 50000}, // 50.0 kHz
    {250, 40000}, // 40.0 kHz
    {320, 31250}, // 31.25 kHz
    {400, 25000}, // 25.0 kHz
    {500, 20000}, // 20.0 kHz
    {625, 16000}, // 16.0 kHz
    {640, 15625}, // 15.625 kHz
    {800, 12500}, // 12.5 kHz
    {1000, 10000}, // 10.0 kHz
    {1250, 8000}, // 8.0 kHz
    {1600, 6250}, // 6.25 kHz
    {2000, 5000}, // 5.0 kHz
    {2500, 4000}, // 4.0 kHz
    {3125, 3200}, // 3.2 kHz
    {3200, 3125}, // 3.125 kHz
    {4000, 2500}, // 2.5 kHz
    {5000, 2000}, // 2.0 kHz
    {6250, 1600}, // 1.6 kHz
    {8000, 1250}, // 1.25 kHz
    {10000, 1000}, // 1.0 kHz
};

/**
 * @brief Структура для хранения данных ШИМ
 * 
 */
typedef struct 
{
    uint8_t pos;   ///< Делитель ШИМ
    frequency_t freq;
} pwm_data_t;


/**
 * @brief Структура для хранения данных элементов
 * 
 */
typedef struct 
{
    pwm_data_t pwm_data [2]; ///< Данные ШИМ
} config_context_t;


static config_context_t s_context = {0}; ///< Данные элементов

inline static int s_calc_pwm_frequency (uint8_t divider);
static void s_config_pwm_channel_str(uint8_t idx, char *value);

/**
 * @brief Инициализация данных элементов
 */
void config_init (void)
{
    s_context.pwm_data[0].pos = 2;
    s_context.pwm_data[1].pos = 2;
}

/**
 * @brief Вычисляет частоту ШИМ
 */
inline static int s_calc_pwm_frequency (uint8_t idx)
{
    if (s_context.pwm_data[idx].pos > sizeof(s_frequencies) / sizeof(frequency_t))
        return -1;
    uint8_t pos = s_context.pwm_data[idx].pos;
    int frequency = s_frequencies[pos].frequency;
    s_context.pwm_data[idx].freq.divider = s_frequencies[pos].divider;
    s_context.pwm_data[idx].freq.frequency = s_frequencies[pos].frequency;
    // printf("%s:%d:\tpos: %u, divider: %d, frequency: %d\n", __FILE__, __LINE__, pos, divider, frequency);
    return frequency;
}

static void s_config_pwm_channel_str(uint8_t idx, char *value)
{
    int frequency = s_calc_pwm_frequency(idx);
    //printf("%s:%d:\tfrequency: %d\n", __FILE__, __LINE__, frequency);

    if (frequency <= 0) {
        snprintf(value, MENU_TITLE_LEN, "Error!");
    } else if (frequency >= 1000000)
    {
        snprintf(value, MENU_TITLE_LEN, "Lo: %.1f MHz", (float)frequency / 1000000.0);
    } else if (frequency >= 1000)
    {
        snprintf(value, MENU_TITLE_LEN, "Lo: %.1f kHz", (float)frequency / 1000.0);
    } else 
    {
        snprintf(value, MENU_TITLE_LEN, "Lo: %.1f Hz", (float)frequency / 1000.0);
    }
}

void config_pwm_hi_channel_str(char *value)
{
    s_config_pwm_channel_str(0, value);
}

void config_pwm_lo_channel_str(char *value)
{
    s_config_pwm_channel_str(1, value);
}

/** 
 * @return void
 */
static void s_set_pwm_divider (uint8_t idx, int delta)
{
    int size = sizeof(s_frequencies) / sizeof(frequency_t);
    int pos = s_context.pwm_data[idx].pos;

    pos += delta;
    while (pos < 0)
        pos += size;
    pos %= size;


    if (pos >=0 && pos < size)
    {
        s_context.pwm_data[idx].pos = pos;
    } else
    {
        s_context.pwm_data[idx].pos = 3;
    }

    // printf("%s:%d\tpos = %d, delta=%d\n", __FILE__, __LINE__, s_context.pwm_data[idx].pos, delta);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void config_set_hi_pwm_freq (int delta)
{
    s_set_pwm_divider(0, delta);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void config_set_lo_pwm_freq (int delta)
{
    s_set_pwm_divider(1, delta);
}


