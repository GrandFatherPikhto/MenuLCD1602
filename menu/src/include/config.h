#include <stdint.h>

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "common.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void config_change_start(bool);
void config_change_ch_hi_duration(int32_t);

#if 0
void config_init (void);
void config_save (void);

void config_pwm_lo_channel_str(char *value, size_t len);
void config_pwm_hi_channel_str(char *value, size_t len);

void config_set_lo_pwm_freq  (int8_t delta);
void config_set_hi_pwm_freq  (int8_t delta);

void config_set_lo_start (int8_t delta);
void config_set_hi_start (int8_t delta);

void config_set_lo_duration (int8_t delta);
void config_set_hi_duration (int8_t delta);

void config_set_lo_enable (int8_t delta);
void config_set_hi_enable (int8_t delta);

void config_set_lo_pwm_duty (int8_t delta);
void config_set_hi_pwm_duty (int8_t delta);

void config_set_lo_pwm_enable (int8_t delta);
void config_set_hi_pwm_enable (int8_t delta)


#ifdef __cplusplus
}
#endif

#endif
#endif // __CONFIG_H__