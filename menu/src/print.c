#include <stdio.h>
#include "menu.h"

static void s_print_menu_tree(const menu_node_t *node, int level);

void print_menu(void)
{
    // const menu_node_t *root = menu_root();
    // const menu_node_t *current = menu_child(root);
    
    printf("┌───────────────────────────────┐\n");
    printf("│          МЕНЮ СИСТЕМЫ         │\n");
    printf("├───────────────────────────────┤\n");
    
    // Рекурсивная функция для печати дерева
    // s_print_menu_tree(root, 0);
    
    printf("└───────────────────────────────┘\n");
}

// Вспомогательная рекурсивная функция для печати дерева
static void s_print_menu_tree(const menu_node_t *node, int level)
{
#if 0    
    if (node == NULL) return;
    
    // Печатаем детей текущего узла
    const menu_node_t *child = menu_child((menu_node_t *)node);
    
    while (child != NULL)
    {
        // Отступ в зависимости от уровня вложенности
        for (int i = 0; i < level; i++)
        {
            if (i == level - 1)
                printf("├── ");
            else
                printf("│   ");
        }
        
        if (level == 0)
            printf("│ ");
        
        // Печатаем название пункта меню
        const char *title = menu_title((menu_node_t *)child);
        if (title)
        {
            if (level == 0)
                printf("● %-27s │\n", title);
            else
                printf("○ %s\n", title);
        }
        
        // Рекурсивно печатаем подменю
        if (menu_child((menu_node_t *)child) != NULL)
        {
            s_print_menu_tree(child, level + 1);
            
            // Печатаем разделитель после подменю
            if (level > 0)
            {
                for (int i = 0; i < level; i++)
                {
                    if (i == level - 1)
                        printf("│   ");
                    else
                        printf("│   ");
                }
                printf("\n");
            }
        }
        
        child = menu_next(child);
    }
    
    if (level == 0)
        printf("│%-31s│\n", "");
#endif
}