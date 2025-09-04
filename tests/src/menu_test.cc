#include <gtest/gtest.h>
#include "menu.h"

// Mock функции для тестирования
static int mock_action_calls = 0;
static void mock_action(int8_t delta) {
    mock_action_calls += delta;
}

static char mock_print_buffer[32];
static void mock_print(char *buf, size_t len) {
    snprintf(buf, len, "MockValue:%d", mock_action_calls);
}

class MenuTest : public ::testing::Test {
protected:
    void SetUp() override {
        menu_init();
        mock_action_calls = 0;
        memset(mock_print_buffer, 0, sizeof(mock_print_buffer));
    }

    void TearDown() override {
        // Очистка ресурсов если используется динамическая память
    }
};

// Тест создания узла
TEST_F(MenuTest, CreateNode) {
    menu_node_t *node = menu_create_node("TestNode", mock_action, mock_print);
    ASSERT_NE(node, nullptr);
    EXPECT_STREQ(menu_title(node), "TestNode");
}

// Тест установки корня
TEST_F(MenuTest, SetRoot) {
    menu_node_t *root = menu_create_node("Root", nullptr, nullptr);
    menu_set_root(root);
    
    EXPECT_EQ(menu_root(), root);
    EXPECT_EQ(menu_current(), root);
}

// Тест добавления дочерних элементов
TEST_F(MenuTest, AddChild) {
    menu_node_t *parent = menu_create_node("Parent", nullptr, nullptr);
    menu_node_t *child1 = menu_create_node("Child1", nullptr, nullptr);
    menu_node_t *child2 = menu_create_node("Child2", nullptr, nullptr);
    
    menu_add_child(parent, child1);
    menu_add_child(parent, child2);
    
    EXPECT_EQ(menu_parent(child1), parent);
    EXPECT_EQ(menu_parent(child2), parent);
    EXPECT_EQ(menu_child(parent), child1);
}

// Тест навигации по меню
TEST_F(MenuTest, Navigation) {
    menu_node_t *root = menu_create_node("Root", nullptr, nullptr);
    menu_node_t *child1 = menu_create_node("Child1", nullptr, nullptr);
    menu_node_t *child2 = menu_create_node("Child2", nullptr, nullptr);
    
    menu_add_child(root, child1);
    menu_add_child(root, child2);
    menu_set_root(root);
    
    // Переход к первому ребенку
    menu_enter();
    EXPECT_EQ(menu_current(), child1);
    
    // Циклическая навигация
    menu_next();
    EXPECT_EQ(menu_current(), child2);
    
    menu_next();
    EXPECT_EQ(menu_current(), child1); // Должен вернуться к первому
    
    menu_prev();
    EXPECT_EQ(menu_current(), child2); // Должен перейти к последнему
    
    // Возврат к родителю
    menu_out();
    EXPECT_EQ(menu_current(), root);
}

// Тест действий меню
TEST_F(MenuTest, ActionHandling) {
    menu_node_t *node = menu_create_node("ActionNode", mock_action, mock_print);
    menu_set_root(node);
    
    // Вход в режим действия
    menu_enter();
    EXPECT_TRUE(menu_is_action(menu_current()));
    
    // Выполнение действия
    menu_handle_delta(5);
    EXPECT_EQ(mock_action_calls, 5);
    
    // Выход из режима действия
    menu_out();
    EXPECT_FALSE(menu_is_action(menu_current()));
}

// Тест печати значений
TEST_F(MenuTest, ValuePrinting) {
    menu_node_t *node = menu_create_node("PrintNode", mock_action, mock_print);
    
    mock_action_calls = 42;
    const char *value = menu_value(node);
    
    EXPECT_STREQ(value, "MockValue:42");
}

// Тест проверки состояний узлов
TEST_F(MenuTest, NodeStates) {
    menu_node_t *root = menu_create_node("Root", nullptr, nullptr);
    menu_node_t *leaf = menu_create_node("Leaf", mock_action, mock_print);
    
    menu_set_root(root);
    
    EXPECT_TRUE(menu_is_root(root));
    EXPECT_FALSE(menu_is_root(leaf));
    
    EXPECT_TRUE(menu_is_leaf(leaf));
    EXPECT_FALSE(menu_is_leaf(root));
}

// Тест циклической навигации с одним элементом
TEST_F(MenuTest, SingleChildNavigation) {
    menu_node_t *root = menu_create_node("Root", nullptr, nullptr);
    menu_node_t *child = menu_create_node("OnlyChild", nullptr, nullptr);
    
    menu_set_root(root);
    menu_add_child(root, child);
    
    menu_enter();
    EXPECT_EQ(menu_current(), child);
    
    // Навигация должна оставаться на том же элементе
    menu_next();
    EXPECT_EQ(menu_current(), child);
    
    menu_prev();
    EXPECT_EQ(menu_current(), child);
}

// Тест обработки нулевых указателей
TEST_F(MenuTest, NullPointerHandling) {
    EXPECT_FALSE(menu_is_root(nullptr));
    EXPECT_FALSE(menu_is_leaf(nullptr));
    EXPECT_FALSE(menu_is_action(nullptr));
    
    EXPECT_STREQ(menu_title(nullptr), "");
    EXPECT_STREQ(menu_value(nullptr), "");
}

// Тест иерархии меню
TEST_F(MenuTest, MenuHierarchy) {
    menu_node_t *root = menu_create_node("Root", nullptr, nullptr);
    menu_node_t *parent = menu_create_node("Parent", nullptr, nullptr);
    menu_node_t *child = menu_create_node("Child", nullptr, nullptr);
    
    menu_set_root(root);
    menu_add_child(root, parent);
    menu_add_child(parent, child);
    
    // Проверка иерархии
    EXPECT_EQ(menu_parent(parent), root);
    EXPECT_EQ(menu_parent(child), parent);
    EXPECT_EQ(menu_child(root), parent);
    EXPECT_EQ(menu_child(parent), child);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}