#include <string.h>
#include <stdio.h>

#include "common.h"
#include "config.h"
#include "console.h"
#include "menu.h"

/**
 * @brief Структура для хранения данных ШИМ
 * 
 */
typedef struct 
{
    uint16_t divider;   ///< Делитель ШИМ
    uint32_t frequency; ///< Частота ШИМ
} pwm_config_t;


/**
 * @brief Структура для хранения данных элементов
 * 
 */
typedef struct 
{
    pwm_config_t pwm_data [2]; ///< Данные ШИМ
} config_data_t;


static config_data_t s_config_data = {0}; ///< Данные элементов

static void s_set_pwm_divider (uint8_t idx, int delta, uint16_t distance);
inline static float s_calc_pwm_frequency (uint16_t divider);
inline static uint16_t s_calc_pwm_divider (uint32_t frequency);

/**
 * @brief Инициализация данных элементов
 */
void config_init (void)
{
    s_config_data.pwm_data[0].divider   = 50;
    s_config_data.pwm_data[0].frequency = s_calc_pwm_frequency(s_config_data.pwm_data[0].divider);
    s_config_data.pwm_data[1].divider   = 50;
    s_config_data.pwm_data[1].frequency = s_calc_pwm_frequency(s_config_data.pwm_data[1].divider);
}

/**
 * @brief Вычисляет частоту ШИМ
 */
inline static float s_calc_pwm_frequency (uint16_t divider)
{
    return (float)CONFIG_PWM_FREQUENCY / ((float)divider*2.0);
}

/**
 * @brief Вычисляет делитель ШИМ
 */
inline static uint16_t s_calc_pwm_divider (uint32_t frequency)
{
    return (uint16_t)(CONFIG_PWM_FREQUENCY / (2.0 * frequency));
}

void config_pwm_lo_channel_freq(char *value)
{
    snprintf(value, MENU_TITLE_LEN, "%u/%.1fus/%uKHz\r\n", 
        s_config_data.pwm_data[0].divider, 
        1000000.0 / s_config_data.pwm_data[0].frequency, 
        s_config_data.pwm_data[0].frequency / 1000);
}

void config_pwm_hi_channel_freq(char *value)
{
    snprintf(value, MENU_TITLE_LEN, "%u/%.1fus/%uKHz\r\n", 
        s_config_data.pwm_data[1].divider,
        1000000.0 / s_config_data.pwm_data[1].frequency, 
        s_config_data.pwm_data[1].frequency / 1000);
}

/** 
 * @brief Устанавливает делитель ШИМ
 * @param delta -- значение на которое изменяется делитель
 * @param distance -- шаг изменения делителя
 * @note
 *    Делитель ШИМ не может быть меньше 2 и больше 10000
 *    Делитель ШИМ должен быть кратен distance
 *    При увеличении делителя, частота ШИМ уменьшается
 *    При уменьшении делителя, частота ШИМ увеличивается
 *    При изменении делителя, частота ШИМ пересчитывается
 *    Печатает частоту ШИМ
 * @return void
 */
static void s_set_pwm_divider (uint8_t idx, int delta, uint16_t distance)
{
    s_config_data.pwm_data[idx].divider = (s_config_data.pwm_data[idx].divider / distance) * distance;  ;
    s_config_data.pwm_data[idx].divider += delta * distance;
    // printf("%s:%d %d %u %u\n", __FILE__, __LINE__, delta, s_config_data.pwm_data[idx].divider, s_config_data.pwm_data[idx].frequency);
    if (s_config_data.pwm_data[idx].divider  > CONFIG_PWM_DIVIDER_MAX)
        s_config_data.pwm_data[idx].divider  = CONFIG_PWM_DIVIDER_MAX;
    if (s_config_data.pwm_data[idx].divider  < CONFIG_PWM_DIVIDER_MIN)
        s_config_data.pwm_data[idx].divider  = CONFIG_PWM_DIVIDER_MIN;

    s_config_data.pwm_data[idx].frequency = s_calc_pwm_frequency(s_config_data.pwm_data[idx].divider);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void config_set_lo_pwm_period01 (int delta)
{
    s_set_pwm_divider(0, delta, 1);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 10
 */
void config_set_lo_pwm_period10 (int delta)
{
    s_set_pwm_divider(0, delta, 10);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 25
 */
void config_set_lo_pwm_period25 (int delta)
{
    s_set_pwm_divider(0, delta, 25);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 50
 */
void config_set_lo_pwm_period50 (int delta)
{
    s_set_pwm_divider(0, delta, 50);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void config_set_hi_pwm_period01 (int delta)
{
    s_set_pwm_divider(1, delta, 1);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 10
 */
void config_set_hi_pwm_period10 (int delta)
{
    s_set_pwm_divider(1, delta, 10);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 25
 */
void config_set_hi_pwm_period25 (int delta)
{
    s_set_pwm_divider(1, delta, 25);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 50
 */
void config_set_hi_pwm_period50 (int delta)
{
    s_set_pwm_divider(1, delta, 50);
}
