#include <gtest/gtest.h>
#include <memory>
#include "common.h"
#include "menu.h"
#include "mock_lcd.h"
#include "mock_rotary.h"

// Mock instances
MockLcd* g_mockLcd = nullptr;
MockRotary* g_mockRotary = nullptr;

class MenuTest : public ::testing::Test {
protected:
    void SetUp() override {
        g_mockLcd = new MockLcd();
        g_mockRotary = new MockRotary();
        menu_init();
    }

    void TearDown() override {
        delete g_mockLcd;
        delete g_mockRotary;
        g_mockLcd = nullptr;
        g_mockRotary = nullptr;
    }

    void CreateTestMenu() {
        root = menu_activate_node(nullptr, "Root", nullptr, nullptr);
        item1 = menu_activate_node(root, "Item1", nullptr, nullptr);
        item2 = menu_activate_node(root, "Item2", nullptr, nullptr);
        item3 = menu_activate_node(root, "Item3", nullptr, nullptr);
        
        sub1 = menu_activate_node(item1, "Sub1", nullptr, nullptr);
        sub2 = menu_activate_node(item1, "Sub2", nullptr, nullptr);
    }

    menu_node_t* root = nullptr;
    menu_node_t* item1 = nullptr;
    menu_node_t* item2 = nullptr;
    menu_node_t* item3 = nullptr;
    menu_node_t* sub1 = nullptr;
    menu_node_t* sub2 = nullptr;
};

// Test 1: Создание структуры меню
TEST_F(MenuTest, MenuStructureCreation) {
    // CreateTestMenu();
    
    // EXPECT_STREQ(menu_get_current_title(), "Root");
    // EXPECT_NE(root, nullptr);
    // EXPECT_NE(item1, nullptr);
    // EXPECT_NE(item2, nullptr);
}

// Test 2: Навигация по меню
TEST_F(MenuTest, MenuNavigation) {
    CreateTestMenu();
    
    // // Начальная позиция
    // EXPECT_STREQ(menu_get_current_title(), "Root");
    
    // // Переход вперед
    // menu_navigate_delta(1);
    // EXPECT_STREQ(menu_get_current_title(), "Item1");
    
    // menu_navigate_delta(1);
    // EXPECT_STREQ(menu_get_current_title(), "Item2");
    
    // // Циклическая навигация
    // menu_navigate_delta(1);
    // EXPECT_STREQ(menu_get_current_title(), "Item3");
    
    // menu_navigate_delta(1);
    // EXPECT_STREQ(menu_get_current_title(), "Root");
    
    // // Назад
    // menu_navigate_delta(-1);
    // EXPECT_STREQ(menu_get_current_title(), "Item3");
}

// Test 3: Вход и выход из подменю
TEST_F(MenuTest, SubmenuNavigation) {
    // CreateTestMenu();
    
    // // Переходим к Item1
    // menu_navigate_delta(1);
    // EXPECT_STREQ(menu_get_current_title(), "Item1");
    
    // // Вход в подменю
    // EXPECT_TRUE(menu_navigate_to_child());
    // EXPECT_STREQ(menu_get_current_title(), "Sub1");
    
    // // Навигация в подменю
    // menu_navigate_delta(1);
    // EXPECT_STREQ(menu_get_current_title(), "Sub2");
    
    // // Выход из подменю
    // EXPECT_TRUE(menu_navigate_to_parent());
    // EXPECT_STREQ(menu_get_current_title(), "Item1");
}

// Test 4: Тестирование действий меню
TEST_F(MenuTest, MenuActions) {
    int callbackValue = 0;
    auto testCallback = [](int delta) { /* mock callback */ };
    
    auto actionItem = menu_activate_node(nullptr, "ActionItem", testCallback, nullptr);
    
    // Проверка наличия действия
    EXPECT_TRUE(menu_has_action());
    
    // Проверка отсутствия действия
    auto noActionItem = menu_activate_node(nullptr, "NoAction", nullptr, nullptr);
    menu_set_current(noActionItem);
    EXPECT_FALSE(menu_has_action());
}