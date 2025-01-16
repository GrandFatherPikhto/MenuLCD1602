#include <stdint.h>

#ifndef __ITEMS_H__
#define __ITEMS_H__

#define PWM_FREQUENCY 10000000 ///< Частота ШИМ
#define ITEM_PWM_DIVIDER_MAX 10000  ///< Максимальное значение делителя ШИМ
#define ITEM_PWM_DIVIDER_MIN 2      ///< Минимальное значение делителя ШИМ
#define ITEM_PWM_FREQUENCY_MAX 1000000 ///< Максимальное значение частоты ШИМ
#define ITEM_PWM_FREQUENCY_MIN 5000    ///< Минимальное значение частоты ШИМ

/**
 * @brief Структура для хранения данных ШИМ
 * 
 */
typedef struct 
{
    uint16_t divider;   ///< Делитель ШИМ
    uint32_t frequency; ///< Частота ШИМ
} pwm_config_t;


void ItemsData_Init (void);

void items_set_lo_pwm_period01  (int delta);
void items_set_lo_pwm_period10  (int delta);
void items_set_lo_pwm_period25  (int delta);
void items_set_lo_pwm_period50  (int delta);
void items_set_hi_pwm_period01  (int delta);
void items_set_hi_pwm_period10  (int delta);
void items_set_hi_pwm_period25  (int delta);
void items_set_hi_pwm_period50  (int delta);

#endif // __ITEMS_H__