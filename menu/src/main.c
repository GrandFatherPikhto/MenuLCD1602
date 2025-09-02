#include "menu.h"
#include "common.h"
#include "print.h"
#include "rotary_encoder.h"
#include "config.h"

int main(void)
{
    menu_init();

    menu_node_t *start = menu_activate_node(NULL, "Start", NULL, NULL);
    menu_node_t *options = menu_activate_node(NULL, "Options", NULL, NULL);

    menu_node_t *channel1 = menu_activate_node(options, "Channel 1", NULL, NULL);
    menu_node_t *enable1 = menu_activate_node(channel1, "Enable", NULL, NULL);
    menu_node_t *pwm1 = menu_activate_node(channel1, "PWM", NULL, NULL);
    menu_node_t *pwm_enable = menu_activate_node(pwm1, "Enable", NULL, NULL);
    menu_node_t *pwm_duty_1 = menu_activate_node(pwm1, "Duty", NULL, NULL);
    menu_activate_node(pwm_duty_1, "x1", NULL, NULL);
    menu_activate_node(pwm_duty_1, "x10", NULL, NULL);
    menu_node_t *pwm_freq_1 = menu_activate_node(pwm1, "Frequency", NULL, NULL);
    menu_activate_node(pwm_freq_1, "x1", &config_set_hi_pwm_period01, &config_pwm_hi_channel_freq);
    menu_activate_node(pwm_freq_1, "x10", &config_set_hi_pwm_period10, &config_pwm_hi_channel_freq);

    menu_node_t *channel2 = menu_activate_node(options, "Channel 2", NULL, NULL);
    menu_node_t *enable2 = menu_activate_node(channel2, "Enable", NULL, NULL);
    menu_node_t *pwm2 = menu_activate_node(channel2, "PWM", NULL, NULL);
    menu_node_t *pwm_enable_2 = menu_activate_node(pwm2, "Enable", NULL, NULL);
    menu_node_t *pwm_duty_2 = menu_activate_node(pwm2, "Duty", NULL, NULL);
    menu_activate_node(pwm_duty_2, "x1", NULL, NULL);
    menu_activate_node(pwm_duty_2, "x10", NULL, NULL);
    menu_node_t * pwm_freq_2 = menu_activate_node(pwm2, "Frequency", NULL, NULL);
    menu_node_t * pwm_freq_2x1 = menu_activate_node(pwm_freq_2, "x1", &config_set_lo_pwm_period01, &config_pwm_lo_channel_freq);
    menu_node_t * pwm_freq_2x10 = menu_activate_node(pwm_freq_2, "x10", &config_set_lo_pwm_period10, &config_pwm_lo_channel_freq);

    // menu_print_items();
    config_init();
    rotary_encoder_init();

    return 0;
}