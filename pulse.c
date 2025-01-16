#include "pulse.h"
#include "items.h"

#define ARM_ENABLE(arm_idx) (1ULL << ((arm_idx == 0) ?  LO_ARM_Pin : HI_ARM_Pin))
#define ARM_DISABLE(arm_idx) (1ULL << ((arm_idx == 0) ? (LO_ARM_Pin + 0x10) : (HI_ARM_Pin + 0x10)))

static uint16_t s_pulse_data[PULSE_DATA_SIZE] = {0}; ///< Данные ШИМ
static pulse_config_t s_pulse_config = {0}; ///< Данные ШИМ

static void s_pulse_recalc (void);

/**
 * @brief Инициализация данных ШИМ
 */
void Pulse_Init (void)
{
}

/**
 * @brief Инициализация данных ШИМ
 * @note 
 *      Реализовать следующую логику для arm_idx-плеча:
 *    - По умолчанию `s_pulse_data[i]` устанавливаем в `LO_HI_DISABLE`
 *    - Если `s_pulse_config.arm[arm_idx].enable` == 1 и `s_pulse_config.arm[opposite_idx].pulse_on` == 0
 *      - Если `s_pulse_config.arm[arm_idx].delay >= i` и `s_pulse_config.arm[arm_idx].duration + s_pulse_config.arm[arm_idx].delay <= i`:
 *          - Если `s_pulse_config.arm[arm_idx].pwm_on` == 1 
 *              - Если `s_pulse_config.arm[arm_idx].counter < s_pulse_config.arm[arm_idx].divider`:
 *                  - Установть для `s_pulse_data[i]` = `LO_ARM_ENABLE` и  `s_pulse_config.arm[arm_idx].pulse_on` = 1 и увеличить `s_pulse_config.arm[arm_idx].counter` на 1
 *              - Если `s_pulse_config.arm[arm_idx].counter >= s_pulse_config.arm[arm_idx].divider`:
 *                  - Инвертировать `s_pulse_config.arm[arm_idx].pulse_on` и сбросить `s_pulse_config.arm[arm_idx].counter` в 0 и установить s_pulse_data[i] = `LO_ARM_DISABLE`
 *          - Если `s_pulse_config.arm[arm_idx].pwm_on` == 0
 *              - Установить для `s_pulse_data[i]` = `LO_ARM_ENABLE` и `s_pulse_config.arm[arm_idx].pulse_on` = 1
 *    - Если `s_pulse_config.arm[arm_idx].enable` == 0
 *          - Установить для `s_pulse_data[i]` = `LO_ARM_DISABLE`
 * @param idx -- индекс в массиве s_pulse_data
 * @param arm_idx -- индекс плеча
 * @param opposite_idx -- индекс противоположного плеча
 * @return void
 **/
static void s_recalc_arm(uint32_t idx, uint8_t arm_idx, uint8_t opposite_idx)
{
    // Обработка arm[arm_idx]
    if (s_pulse_config.arm[arm_idx].enable && !s_pulse_config.arm[opposite_idx].pulse_on)
    {
        if (idx >= s_pulse_config.arm[arm_idx].delay &&
            idx < s_pulse_config.arm[arm_idx].duration + s_pulse_config.arm[arm_idx].delay)
        {
            if (s_pulse_config.arm[arm_idx].pwm_on)
            {
                if (s_pulse_config.arm[arm_idx].counter < s_pulse_config.arm[arm_idx].divider)
                {
                    s_pulse_data[idx] |=  ARM_ENABLE(arm_idx);
                    s_pulse_data[idx] &= ~ARM_DISABLE(arm_idx);
                    s_pulse_config.arm[arm_idx].pulse_on = 1;
                    s_pulse_config.arm[arm_idx].counter++;
                } else
                {
                    s_pulse_config.arm[arm_idx].pulse_on ^= 1; // Инвертируем состояние
                    s_pulse_config.arm[arm_idx].counter = 0;
                    s_pulse_data[idx] |= ARM_DISABLE(arm_idx);
                    s_pulse_data[idx] &= ~ARM_ENABLE(arm_idx);
                }
            } else if (s_pulse_config.arm[opposite_idx].pulse_on == 0)
            {
                s_pulse_data[idx] |=  ARM_ENABLE(arm_idx);
                s_pulse_data[idx] &= ~ARM_DISABLE(arm_idx);
                s_pulse_config.arm[arm_idx].pulse_on = 1;
            }
        } else
        {
            s_pulse_data[idx] |=  ARM_DISABLE(arm_idx);
            s_pulse_data[idx] &= ~ARM_ENABLE (arm_idx);
            s_pulse_config.arm[arm_idx].pulse_on = 0;    
        }
    }
}

/**
 * @brief Инициализация данных ШИМ
 **/
static void s_pulse_recalc (void)
{
    for (uint32_t idx = 0; idx < PULSE_DATA_SIZE; idx++)
    {
        // По умолчанию устанавливаем LO/HI Disable
        s_pulse_data[idx] |= ARM_DISABLE(0) | ARM_DISABLE(1);
        s_pulse_data[idx] &= ~(ARM_ENABLE(0) | ARM_ENABLE(1));

        // Обработка arm_lo
        s_recalc_arm(idx, 0, 1);
        // Обработка arm_hi
        s_recalc_arm(idx, 1, 0);
   }
}