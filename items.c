#include <string.h>
#include <stdio.h>

#include "items.h"
#include "console.h"
#include "menu.h"

/**
 * @brief Структура для хранения данных элементов
 * 
 */
typedef struct 
{
    pwm_config_t pwm_data [2]; ///< Данные ШИМ
} items_data_t;


static items_data_t s_items_data = {0}; ///< Данные элементов

static void s_set_pwm_divider (uint8_t idx, int delta, uint16_t distance);
inline static float s_calc_pwm_frequency (uint16_t divider);
inline static uint16_t s_calc_pwm_divider (uint32_t frequency);
inline static void s_print_pwm_frequency (uint8_t idx);

/**
 * @brief Инициализация данных элементов
 */
void ItemsData_Init (void)
{
    s_items_data.pwm_data[0].divider   = 50;
    s_items_data.pwm_data[0].frequency = s_calc_pwm_frequency(s_items_data.pwm_data[0].divider);
    s_items_data.pwm_data[1].divider   = 50;
    s_items_data.pwm_data[1].frequency = s_calc_pwm_frequency(s_items_data.pwm_data[1].divider);
}

/**
 * @brief Вычисляет частоту ШИМ
 */
inline static float s_calc_pwm_frequency (uint16_t divider)
{
    return (float)PWM_FREQUENCY / ((float)divider*2.0);
}

/**
 * @brief Вычисляет делитель ШИМ
 */
inline static uint16_t s_calc_pwm_divider (uint32_t frequency)
{
    return (uint16_t)(PWM_FREQUENCY / (2.0 * frequency));
}

/**
 * @brief Печатает частоту ШИМ
 */
inline static void s_print_pwm_frequency (uint8_t idx)
{
    printf("\033[H\033[J");
    printf(">%s Frequency<\r\n", idx == 0 ? "LO" : "HI");
    printf("%u/%.1fus/%uKHz\r\n", s_items_data.pwm_data[idx].divider, 1000000.0 / s_items_data.pwm_data[idx].frequency, s_items_data.pwm_data[idx].frequency / 1000);
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
    s_items_data.pwm_data[idx].divider = (s_items_data.pwm_data[idx].divider / distance) * distance;  ;
    s_items_data.pwm_data[idx].divider += delta * distance;
    if (s_items_data.pwm_data[idx].divider  > ITEM_PWM_DIVIDER_MAX)
        s_items_data.pwm_data[idx].divider  = ITEM_PWM_DIVIDER_MAX;
    if (s_items_data.pwm_data[idx].divider  < ITEM_PWM_DIVIDER_MIN)
        s_items_data.pwm_data[idx].divider  = ITEM_PWM_DIVIDER_MIN;

    s_items_data.pwm_data[idx].frequency = s_calc_pwm_frequency(s_items_data.pwm_data[idx].divider);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void items_set_lo_pwm_period01 (int delta)
{
    s_set_pwm_divider(0, delta, 1);
    s_print_pwm_frequency(0);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 10
 */
void items_set_lo_pwm_period10 (int delta)
{
    s_set_pwm_divider(0, delta, 10);
    s_print_pwm_frequency(0);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 25
 */
void items_set_lo_pwm_period25 (int delta)
{
    s_set_pwm_divider(0, delta, 25);
    s_print_pwm_frequency(0);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 50
 */
void items_set_lo_pwm_period50 (int delta)
{
    s_set_pwm_divider(0, delta, 50);
    s_print_pwm_frequency(0);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 01
 */
void items_set_hi_pwm_period01 (int delta)
{
    s_set_pwm_divider(1, delta, 1);
    s_print_pwm_frequency(1);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 10
 */
void items_set_hi_pwm_period10 (int delta)
{
    s_set_pwm_divider(1, delta, 10);
    s_print_pwm_frequency(1);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 25
 */
void items_set_hi_pwm_period25 (int delta)
{
    s_set_pwm_divider(1, delta, 25);
    s_print_pwm_frequency(1);
}

/**
 * @brief Устанавливает частоту ШИМ c шагом 50
 */
void items_set_hi_pwm_period50 (int delta)
{
    s_set_pwm_divider(1, delta, 50);
    s_print_pwm_frequency(1);
}
