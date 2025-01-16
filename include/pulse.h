#include <stdint.h>

#include "items.h"

#ifndef __PULSE_H__
#define __PULSE_H__

#define PULSE_DATA_SIZE 0x4000
#define LO_ARM_Pin 11
#define HI_ARM_Pin 12

typedef struct
{
    uint32_t delay; ///< Задержка
    uint32_t duration; ///< Длительность
    uint16_t divider; ///< Делитель ШИМ
    uint16_t counter; ///< Счётчик
    uint8_t enable: 1, pwm_on: 1, pulse_on: 1; ///< Флаги
} arm_config_t;

typedef struct 
{
    arm_config_t arm[0]; ///< Данные ШИМ
    pwm_config_t pwm; ///< Данные ШИМ
} pulse_config_t;

void Pulse_Init (void);

#endif // __PULSE_H__