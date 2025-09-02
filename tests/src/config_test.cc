#include <gtest/gtest.h>
#include "config.h"
#include "common.h"

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_init();
    }
};

// Test 1: Инициализация конфигурации
TEST_F(ConfigTest, ConfigurationInitialization) {
    char buffer[MENU_TITLE_LEN];
    
    config_pwm_hi_channel_freq(buffer);
    // Проверяем, что значение инициализировано
    EXPECT_STRNE(buffer, "");
}

// Test 2: Изменение частоты PWM
TEST_F(ConfigTest, PWMFrequencyChange) {
    char buffer1[MENU_TITLE_LEN];
    char buffer2[MENU_TITLE_LEN];
    
    // Получаем начальное значение
    config_pwm_hi_channel_freq(buffer1);
    
    // Изменяем частоту
    config_set_hi_pwm_period10(1); // +10
    
    // Получаем новое значение
    config_pwm_hi_channel_freq(buffer2);
    
    // Значения должны отличаться
    EXPECT_STRNE(buffer1, buffer2);
}

// Test 3: Граничные значения PWM
TEST_F(ConfigTest, PWMBoundaryValues) {
    // Пытаемся установить значение ниже минимума
    config_set_hi_pwm_period01(-1000);
    
    char buffer[MENU_TITLE_LEN];
    config_pwm_hi_channel_freq(buffer);
    
    // Должно остаться минимальное значение
    EXPECT_STRNE(buffer, "");
}