#include <stdint.h>

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

void config_init (void);

void config_pwm_lo_channel_str(char *value);
void config_pwm_hi_channel_str(char *value);

void config_set_lo_pwm_freq  (int delta);
void config_set_hi_pwm_freq  (int delta);

#ifdef __cplusplus
}
#endif

#endif // __CONFIG_H__