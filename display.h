#ifndef _DISPLAY_H
#define _DISPLAY_H

#include "menu.h"

#include <string>
#include <stack>
#include <utility>

class Display{
public:
    Display();
    ~Display();
    void print_text(const std::string& text);
    void set_menu(Menu new_menu);
    void draw_menu();
    MenuItem get_item();
    void jump_to(std::pair<unsigned int, unsigned int> indices);
    int get_display_width();
    bool get_string(const char* prompt, std::string& result);
    bool get_string(const std::string& prompt, std::string& result);
    std::pair<unsigned int, unsigned int> get_last_sel();
private:
    void move_cursor(int amount);
    Menu items;
    unsigned int selected_item, region_start;
    int wind_x, wind_y;
    unsigned int last_selected_item, last_region_start;
    bool is_printable(char c);
};

#endif
