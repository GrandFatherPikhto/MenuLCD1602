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
    const menu_node_t *last = menu_child_last((menu_node_t *)node);
    const menu_node_t *prev = child;

    // printf("Child: %s, Last: %s, %d\n", menu_title(child), menu_title(last), child == last);
    uint8_t counter = 0;
    
    while (prev != last || counter > 128)
    {
        // Отступ в зависимости от уровня вложенности
        for (int i = 0; i < level; i++)
        {
            if (i != 0 && i == level - 1 && child == last)
                printf("└── ");
            else if (i == level - 1)
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
        prev = child;       
        child = menu_get_next(child);
        counter ++;
    }
    
    if (level == 0)
        printf("│%-31s│\n", "");
#endif        
}