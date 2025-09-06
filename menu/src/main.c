#include "menu.h"
#include "common.h"
#include "current.h"
#include "rotary_encoder.h"
#include "config.h"
#include "lcd1602.h"
#include "strategy_submenu.h"
#include "strategy_action.h"
#include "strategy_toggle.h"
#include "strategy_numeric.h"

int main(void)
{
    static menu_handle_t *menu_handle = 0;
    menu_init(&menu_handle);

    menu_node_t *main_node = menu_create_node(menu_handle, "Main", &MENU_SUBMENU_STRATEGY);
    menu_node_t *start_node = menu_create_node(menu_handle, "Start", &MENU_SUBMENU_STRATEGY);
    menu_node_t *start_toggle_node = menu_create_node(menu_handle, "Start", &MENU_TOGGLE_STRATEGY);
    menu_node_t *options_node = menu_create_node(menu_handle, "Options", &MENU_SUBMENU_STRATEGY);

    menu_node_t *ch1_node = menu_create_node(menu_handle, "Channel 1", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch1_enable_node = menu_create_node(menu_handle, "Enable", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch1_enable_toggle = menu_create_node(menu_handle, "Toggle", &MENU_TOGGLE_STRATEGY);
    menu_node_t *ch1_duration_node = menu_create_node(menu_handle, "Duration", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch1_duration_change = menu_create_node(menu_handle, "Duration", &MENU_NUMERIC_STRATEGY);
    //menu_node_t *ch1_start_node = menu_create_node(menu_handle, "Start", &MENU_SUBMENU_STRATEGY);
    // menu_node_t *ch1_pwm_node = menu_create_node(menu_handle, "PWM", &MENU_SUBMENU_STRATEGY);
    // menu_node_t *ch1_pwm_enable_node = menu_create_node(menu_handle, "Enable", &MENU_SUBMENU_STRATEGY);
    // menu_node_t *ch1_pwm_duty_node = menu_create_node(menu_handle, "Duty", &MENU_SUBMENU_STRATEGY);
    // menu_node_t *ch1_pwm_freq_node = menu_create_node(menu_handle, "Frequency", &MENU_SUBMENU_STRATEGY);

    menu_node_t *ch2_node = menu_create_node(menu_handle, "Channel 2", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch2_enable_node = menu_create_node(menu_handle, "Enable", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch2_start_node = menu_create_node(menu_handle, "Start", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch2_duration_node = menu_create_node(menu_handle, "Duration", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch2_pwm_node = menu_create_node(menu_handle, "PWM", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch2_pwm_enable_node = menu_create_node(menu_handle, "Enable", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch2_pwm_duty_node = menu_create_node(menu_handle, "Duty", &MENU_SUBMENU_STRATEGY);
    menu_node_t *ch2_pwm_freq_node = menu_create_node(menu_handle, "Frequency", &MENU_SUBMENU_STRATEGY);

    menu_add_child(menu_handle, main_node, start_node);
    menu_add_child(menu_handle, main_node, options_node);

    toggle_strategy_data_t start_data = { 0 };
    start_data.change_cb = &config_change_start;
    SET_FLAG(start_data.flags, TOGGLE_STRATEGY_FLAG_DISABLE_ON_EXIT);
    menu_node_set_userdata(start_toggle_node, (void *)&start_data);

    menu_add_child(menu_handle, start_node, start_toggle_node);

    int32_t duration = 1000;
    numeric_data_t ch1_duration_data = {
        .change_callback = &config_change_ch_hi_duration,
        .current_factor_idx = 0,
        .value_ptr = &duration,
        .min_value = 100,
        .max_value = 0x2000
    };
    menu_node_set_userdata(ch1_duration_change, &ch1_duration_data);

    menu_add_child(menu_handle, options_node, ch1_node);
    menu_add_child(menu_handle, ch1_node, ch1_enable_node);
    menu_add_child(menu_handle, ch1_enable_node, ch1_enable_toggle);
    menu_add_child(menu_handle, ch1_node, ch1_duration_node);
    menu_add_child(menu_handle, ch1_duration_node, ch1_duration_change);
    // menu_add_child(menu_handle, ch1_node, ch1_start_node);
    // menu_add_child(menu_handle, ch1_node, ch1_pwm_node);
    // menu_add_child(menu_handle, ch1_node, ch1_pwm_node);
    // menu_add_child(menu_handle, ch1_pwm_node, ch1_pwm_enable_node);
    // menu_add_child(menu_handle, ch1_pwm_node, ch1_pwm_duty_node);
    // menu_add_child(menu_handle, ch1_pwm_node, ch1_pwm_freq_node);

    menu_add_child(menu_handle, options_node, ch2_node);
    menu_add_child(menu_handle, ch2_node, ch2_enable_node);
    menu_add_child(menu_handle, ch2_node, ch2_start_node);
    menu_add_child(menu_handle, ch2_node, ch2_duration_node);
    menu_add_child(menu_handle, ch2_node, ch2_pwm_node);
    menu_add_child(menu_handle, ch2_pwm_node, ch2_pwm_enable_node);
    menu_add_child(menu_handle, ch2_pwm_node, ch2_pwm_duty_node);
    // menu_add_child(menu_handle, ch2_pwm_node, ch2_pwm_freq_node);

    menu_set_current(menu_handle, start_node);

    rotary_encoder_init(menu_handle);
    init_current(menu_handle);
    lcd1602_init (
        title_str,
        value_str,
        rotary_encoder_callback, 
        rotary_encoder_push_button_callback, 
        rotary_encoder_long_push_button_callback
    );


    return 0;
}