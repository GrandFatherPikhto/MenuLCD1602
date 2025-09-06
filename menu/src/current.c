#include "current.h"
#include "menu.h"

menu_handle_t *s_menu_handle = 0;

void init_current(menu_handle_t *handle)
{
    s_menu_handle = handle;
}

const char *title_str(void)
{
    static char buf[MENU_TITLE_LEN] = {0};
    memset(buf, 0, MENU_TITLE_LEN);
    if (s_menu_handle == 0)
        return "";
    menu_title_str(s_menu_handle, 0, buf, MENU_TITLE_LEN);
    return buf;
}

const char *value_str(void)
{
    static char buf[MENU_TITLE_LEN] = {0};
    memset(buf, 0, MENU_TITLE_LEN);
    if (s_menu_handle == 0)
        return "";
    menu_value_str(s_menu_handle, 0, buf, MENU_TITLE_LEN);
    return buf;
}