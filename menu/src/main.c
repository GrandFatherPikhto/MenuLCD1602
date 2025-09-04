#include "menu.h"
#include "common.h"
#include "print.h"
#include "rotary_encoder.h"
#include "config.h"
#include "lcd1602.h"

int main(void)
{
    menu_init();

    menu_node_t *root_node = menu_create_node("Main", NULL, NULL);

    menu_node_t *start_node = menu_create_node("Start", NULL, NULL);
    menu_node_t *options_node = menu_create_node("Options", NULL, NULL);

    menu_node_t *ch1_node = menu_create_node("Channel 1", NULL, NULL);
    menu_node_t *ch1_enable_node = menu_create_node("Enable", NULL, NULL);
    menu_node_t *ch1_pwm_node = menu_create_node("PWM", NULL, NULL);
    menu_node_t *ch1_pwm_enable_node = menu_create_node("Enable", NULL, NULL);
    menu_node_t *ch1_pwm_duty_node = menu_create_node("Duty", NULL, NULL);
    menu_node_t *ch1_pwm_freq_node = menu_create_node( 
        "Frequency", 
        &config_set_hi_pwm_freq, 
        &config_pwm_hi_channel_str);    

    menu_node_t *ch2_node = menu_create_node("Channel 2", NULL, NULL);
    menu_node_t *ch2_enable_node = menu_create_node("Enable", NULL, NULL);
    menu_node_t *ch2_pwm_node = menu_create_node("PWM", NULL, NULL);
    menu_node_t *ch2_pwm_enable_node = menu_create_node("Enable", NULL, NULL);
    menu_node_t *ch2_pwm_duty_node = menu_create_node("Duty", NULL, NULL);
    menu_node_t *ch2_pwm_freq_node = menu_create_node( 
        "Frequency", 
        &config_set_lo_pwm_freq, 
        &config_pwm_lo_channel_str);

    menu_set_root(root_node); // Только в конце, чтобы активировать первую дочернюю ноду от рута

    menu_add_child(root_node, start_node);
    menu_add_child(root_node, options_node);

    menu_add_child(options_node, ch1_node);
    menu_add_child(ch1_node, ch1_pwm_node);
    menu_add_child(ch1_node, ch1_enable_node);
    menu_add_child(ch1_node, ch1_pwm_node);
    menu_add_child(ch1_pwm_node, ch1_pwm_enable_node);
    menu_add_child(ch1_pwm_node, ch1_pwm_duty_node);
    menu_add_child(ch1_pwm_node, ch1_pwm_freq_node);

    menu_add_child(options_node, ch2_node);
    menu_add_child(ch2_node, ch2_pwm_node);
    menu_add_child(ch2_node, ch2_enable_node);
    menu_add_child(ch2_node, ch2_pwm_node);
    menu_add_child(ch2_pwm_node, ch2_pwm_enable_node);
    menu_add_child(ch2_pwm_node, ch2_pwm_duty_node);
    menu_add_child(ch2_pwm_node, ch2_pwm_freq_node);

    menu_set_current(start_node);

    print_menu();
    config_init();
    rotary_encoder_init();
    lcd1602_init (
        rotary_encoder_callback, 
        rotary_encoder_push_button_callback, 
        rotary_encoder_long_push_button_callback
    );

    return 0;
}