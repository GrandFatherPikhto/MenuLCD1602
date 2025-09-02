#include <gtest/gtest.h>
#include "common.h"
#include "rotary_encoder.h"
#include "menu.h"
#include "mock_lcd.h"

class RotaryTest : public ::testing::Test {
protected:
    void SetUp() override {
        g_mockLcd = new MockLcd();
        menu_init();
        rotary_encoder_init();
    }

    void TearDown() override {
        delete g_mockLcd;
        g_mockLcd = nullptr;
    }
};

// Test 1: Обработка поворота энкодера
TEST_F(RotaryTest, EncoderRotation) {
    // Ожидаем вызов отображения меню
    EXPECT_CALL(*g_mockLcd, printMenu(testing::_, testing::_)).Times(2);
    
    // Симулируем поворот энкодера
    rotary_encoder_callback(2); // +1 шаг
}

// Test 2: Обработка нажатия кнопки
TEST_F(RotaryTest, ButtonPress) {
    EXPECT_CALL(*g_mockLcd, printMenu(testing::_, testing::_)).Times(2);
    
    push_button_callback();
}

// Test 3: Фильтрация шумов энкодера
TEST_F(RotaryTest, EncoderNoiseFiltering) {
    // Эти вызовы должны быть проигнорированы из-за фильтра
    rotary_encoder_callback(1); // Меньше фильтра
    rotary_encoder_callback(3); // Не кратно фильтру
    
    // Этот вызов должен пройти
    EXPECT_CALL(*g_mockLcd, printMenu(testing::_, testing::_));
    rotary_encoder_callback(4); // Кратно фильтру (2 * 2)
}