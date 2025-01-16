#include <stdint.h>

#define PULSE_DATA_SIZE 0x4000
#define LO_ARM_Pin 11
#define HI_ARM_Pin 12

typedef struct 
{
    uint16_t divider;   ///< Делитель ШИМ
    uint32_t frequency; ///< Частота ШИМ
} pwm_config_t;


typedef struct
{
    uint32_t delay; ///< Задержка
    uint32_t duration; ///< Длительность
    uint16_t divider; ///< Делитель ШИМ
    uint16_t counter; ///< Счётчик
    uint8_t enable: 1, pwm: 1, pulse_on: 1, pulse_back: 1; ///< Флаги
} arm_config_t;

typedef struct 
{
    arm_config_t arm_lo; ///< Данные ШИМ
    arm_config_t arm_hi; ///< Данные ШИМ
    pwm_config_t pwm; ///< Данные ШИМ
} pulse_config_t;

#define LO_ARM_ENABLE (1ULL << LO_ARM_Pin)
#define HI_ARM_ENABLE (1ULL << HI_ARM_Pin)
#define LO_ARM_DISABLE (1ULL << (LO_ARM_Pin + 0x10))
#define HI_ARM_DISABLE (1ULL << (HI_ARM_Pin + 0x10))
#define LO_HI_TOGGLE_LO (LO_ARM_ENABLE | HI_ARM_DISABLE)
#define LO_HI_TOGGLE_HI (LO_ARM_DISABLE | HI_ARM_ENABLE)
#define LO_HI_DISABLE (LO_ARM_DISABLE | HI_ARM_DISABLE)

static uint16_t s_pulse_data[PULSE_DATA_SIZE] = {0}; ///< Данные ШИМ
static pulse_config_t s_pulse_config = {0}; ///< Данные ШИМ

static void s_pulse_recalc (void);

/**
 * @brief Инициализация данных ШИМ
 * @note 
 *      Реализовать следующую логику:
 *    - По умолчанию `s_pulse_data[i]` устанавливаем в `LO_HI_DISABLE`
 *    - Если `s_pulse_config.arm_lo.enable` == 1 и `s_pulse_config.arm_hi.pulse_on` == 0
 *      - Если `s_pulse_config.arm_lo.delay >= i` и `s_pulse_config.arm_lo.duration + s_pulse_config.arm_lo.delay <= i`:
 *          - Если `s_pulse_config.arm_lo.pwm` == 1 
 *              - Если `s_pulse_config.arm_lo.counter < s_pulse_config.arm_lo.divider`:
 *                  - Установть для `s_pulse_data[i]` = `LO_ARM_ENABLE` и  `s_pulse_config.arm_lo.pulse_on` = 1 и увеличить `s_pulse_config.arm_lo.counter` на 1
 *              - Если `s_pulse_config.arm_lo.counter >= s_pulse_config.arm_lo.divider`:
 *                  - Инвертировать `s_pulse_config.arm_lo.pulse_on` и сбросить `s_pulse_config.arm_lo.counter` в 0 и установить s_pulse_data[i] = `LO_ARM_DISABLE`
 *          - Если `s_pulse_config.arm_lo.pwm` == 0
 *              - Установить для `s_pulse_data[i]` = `LO_ARM_ENABLE` и `s_pulse_config.arm_lo.pulse_on` = 1
 *    - Если `s_pulse_config.arm_lo.enable` == 0
 *          - Установить для `s_pulse_data[i]` = `LO_ARM_DISABLE`
 * 
 *    - Если `s_pulse_config.arm_hi.enable` == 1 и `s_pulse_config.arm_lo.pulse_on` == 0
 *      - Если `s_pulse_config.arm_hi.delay >= i` и `s_pulse_config.arm_hi.duration + s_pulse_config.arm_hi.delay <= i`:
 *          - Если `s_pulse_config.arm_hi.pwm` == 1 
 *              - Если `s_pulse_config.arm_hi.counter < s_pulse_config.arm_hi.divider`:
 *                  - Установть для `s_pulse_data[i]` = `HI_ARM_ENABLE` и  `s_pulse_config.arm_hi.pulse_on` = 1 и увеличить `s_pulse_config.arm_hi.counter` на 1
 *              - Если `s_pulse_config.arm_hi.counter >= s_pulse_config.arm_hi.divider`:
 *                  - Инвертировать `s_pulse_config.arm_hi.pulse_on` и сбросить `s_pulse_config.arm_hi.counter` в 0 и установить s_pulse_data[i] = `LO_ARM_DISABLE`
 *          - Если `s_pulse_config.arm_hi.pwm` == 0
 *              - Установить для `s_pulse_data[i]` = `HI_ARM_ENABLE` и `s_pulse_config.arm_hi.pulse_on` = 1
 *    - Если `s_pulse_config.arm_hi.enable` == 0
 *          - Установить для `s_pulse_data[i]` = `HI_ARM_DISABLE`
 **/
static void s_pulse_recalc (void)
{
    for (uint16_t i = 0; i < PULSE_DATA_SIZE; i++)
    {
        // По умолчанию устанавливаем LO_HI_DISABLE
        s_pulse_data[i] = LO_HI_DISABLE;

        // Обработка arm_lo
        if (s_pulse_config.arm_lo.enable && !s_pulse_config.arm_hi.pulse_on)
        {
            if (i >= s_pulse_config.arm_lo.delay &&
                i < s_pulse_config.arm_lo.duration + s_pulse_config.arm_lo.delay)
            {
                if (s_pulse_config.arm_lo.pwm)
                {
                    if (s_pulse_config.arm_lo.counter < s_pulse_config.arm_lo.divider)
                    {
                        s_pulse_data[i] = LO_ARM_ENABLE;
                        s_pulse_config.arm_lo.pulse_on = 1;
                        s_pulse_config.arm_lo.counter++;
                    }
                    else
                    {
                        s_pulse_config.arm_lo.pulse_on ^= 1; // Инвертируем состояние
                        s_pulse_config.arm_lo.counter = 0;
                        s_pulse_data[i] = LO_ARM_DISABLE;
                    }
                }
                else
                {
                    s_pulse_data[i] = LO_ARM_ENABLE;
                    s_pulse_config.arm_lo.pulse_on = 1;
                }
            }
            else
            {
                s_pulse_data[i] = LO_ARM_DISABLE;
            }
        }

        // Обработка arm_hi
        if (s_pulse_config.arm_hi.enable && !s_pulse_config.arm_lo.pulse_on)
        {
            if (i >= s_pulse_config.arm_hi.delay &&
                i < s_pulse_config.arm_hi.duration + s_pulse_config.arm_hi.delay)
            {
                if (s_pulse_config.arm_hi.pwm)
                {
                    if (s_pulse_config.arm_hi.counter < s_pulse_config.arm_hi.divider)
                    {
                        s_pulse_data[i] = HI_ARM_ENABLE;
                        s_pulse_config.arm_hi.pulse_on = 1;
                        s_pulse_config.arm_hi.counter++;
                    }
                    else
                    {
                        s_pulse_config.arm_hi.pulse_on ^= 1; // Инвертируем состояние
                        s_pulse_config.arm_hi.counter = 0;
                        s_pulse_data[i] = HI_ARM_DISABLE;
                    }
                }
                else
                {
                    s_pulse_data[i] = HI_ARM_ENABLE;
                    s_pulse_config.arm_hi.pulse_on = 1;
                }
            }
            else
            {
                s_pulse_data[i] = HI_ARM_DISABLE;
            }
        }
    }
}