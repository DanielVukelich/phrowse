#include "display.h"

#include <ncurses.h>

#define ALT_BACKSPACE 127
#define EOT 4

Display::Display(){
    initscr();
    wind_y = stdscr->_maxy + 1;
    raw();
    noecho();
    keypad(stdscr, true);
    selected_item = 0;
    region_start = 0;
}

Display::~Display(){
    endwin();
}

void Display::print_prompt(){
    printw(">");
}

MenuItem Display::get_item(){
    bool getting_input = true;
    while(getting_input){
        int ch = getch();
        switch(ch){
        case KEY_DOWN:
            move_cursor(1);
            break;
        case KEY_UP:
            move_cursor(-1);
            break;
        case KEY_PPAGE:
            move_cursor(-1 * wind_y);
            break;
        case KEY_NPAGE:
            move_cursor(wind_y);
            break;
        case '\n':
            getting_input = false;
            break;
        case EOT:
            return Menu::no_item;
        default:
            break;
        }

        draw_menu();
    }
    return items.at(selected_item);
}

bool Display::get_line(std::string& dest){
    int ch = 0;
    dest.clear();
    while((ch = getch()) != '\n' && ch != EOT){
        if(ch == KEY_BACKSPACE || ch == ALT_BACKSPACE){
            if(dest.size()){
                dest.pop_back();
            }
        }else{
            dest.push_back(ch);
        }
    }
    return ch != EOT;
}

void Display::draw_menu(){
    clear();

    for(unsigned int i = region_start, j = 0;
        i < items.size() && j != (unsigned int) wind_y;
        ++i, ++j){

        if(selected_item == i){
            attron(A_REVERSE);
        }
        MenuItem it = items.at(i);
        printw(it.get_text().c_str());
        printw("\n");
        standend();
    }
    refresh();
}

void Display::set_menu(Menu new_menu){
    items = new_menu;
    selected_item = 0;
}

void Display::move_cursor(int amount){

    int temp_sel_item = (int) selected_item + amount;
    if(temp_sel_item < 0){
        selected_item = 0;
    }else if(temp_sel_item > (int) items.size()){
        selected_item = items.size() - 1;
    }else{
        selected_item = (unsigned int) temp_sel_item;
    }

    int selection_diff = (int) selected_item - (int) region_start;
    if(selection_diff < 0){
        region_start = selected_item;
    }else if(selection_diff >= wind_y){
        region_start += amount;
    }
}
