/*
 * lefl_menu.c
 */
#include "lefl.h"

void lefl_menu_init(lefl_menu_t* menu, const char* *items,uint8_t len,void (*cb)(void* menu))
{
    menu->items=items;
    menu->len=len;
    menu->menu_cb=cb;
    menu->selected_index=0;
}

void lefl_menu_index_increase(lefl_menu_t *menu, int8_t delta)
{
    menu->selected_index+=delta;
    if(menu->selected_index>=menu->len)
    {
        menu->selected_index=0;
    }
    if(menu->selected_index<0)
    {
        menu->selected_index=menu->len-1;
    }
}
void lefl_menu_click(lefl_menu_t *menu)
{
    if(menu->menu_cb!=NULL)
        menu->menu_cb(menu);
}


