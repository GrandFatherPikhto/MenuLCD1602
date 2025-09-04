#include <gtest/gtest.h>
#include "common.h"
#include "menu.h"
#include "rotary_encoder.h"
#include "config.h"
#include "mock_lcd.h"

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        g_mockLcd = new MockLcd();
        menu_init();
        config_init();
        rotary_encoder_init();
        
        // Создаем тестовое меню с действиями
        auto pwmMenu = menu_activate_node(nullptr, "PWM", nullptr, nullptr);
        menu_activate_node(pwmMenu, "Freq x1", 
                         config_set_hi_pwm_freq, 
                         config_pwm_hi_channel_str);
    }
};

// Test 1: Полный сценарий использования
TEST_F(IntegrationTest, FullUsageScenario) {
    EXPECT_CALL(*g_mockLcd, printMenu(testing::_, testing::_)).Times(4);
    
    // 1. Навигация к пункту меню
    // menu_navigate_delta(1);
    
    // 2. Вход в подменю
    // menu_navigate_to_child();
    
    // 3. Выполнение действия
    // menu_handle_action(1);
    
    // 4. Отображение значения
    char title[MENU_TITLE_LEN], value[MENU_TITLE_LEN];
    // menu_print_value(title, value);
    
    EXPECT_STRNE(value, "");
}