#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "menu.h"

#include <string>
#include <stack>

class Display{
public:
    Display();
    ~Display();
    void set_menu(Menu new_menu);
    void draw_menu();
    void print_prompt();
    MenuItem get_item();
    bool get_line(std::string& dest);
private:
    void move_cursor(int amount);
    Menu items;
    unsigned int selected_item;
    int wind_x, wind_y;
    unsigned int region_start;
};

#endif
