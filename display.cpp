#include "display.h"

#include <ncurses.h>
#include <string>
#include <sstream>
#include <unistd.h>

#define ALT_BACKSPACE 127
#define EOT 4

using std::ostringstream;
using std::string;
using std::pair;
using std::make_pair;

Display::Display(){
    initscr();
    wind_y = stdscr->_maxy;
    wind_x = stdscr->_maxx;
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
        case KEY_LEFT:
            last_selected_item = selected_item;
            last_region_start = region_start;
            return Menu::prev_item;
        case KEY_RIGHT:
            return Menu::next_item;
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
            if(items.at(selected_item).get_type() != 'i'){
                getting_input = false;
            }
            break;
        case EOT:
            return Menu::no_item;
        default:
            break;
        }

        draw_menu();
    }
    last_selected_item = selected_item;
    last_region_start = region_start;
    return items.at(selected_item);
}

pair<unsigned int, unsigned int> Display::get_last_sel(){
    return make_pair(last_selected_item, last_region_start);
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
        string it = items.at(i).get_text();
        for(unsigned int k = 0; k < it.length(); ++k){
            char c = it.at(k);
            if(is_printable(c)){
                addch(c);
            }
        }
        printw("\n");
        standend();
    }

    ostringstream oss;
    oss << "Selected item: ";
    oss << items.at(selected_item).describe_item() << " | ";
    oss << "Line " << selected_item + 1 << "/" << items.size();
#ifdef DEBUG
    oss << " | Debug PID: " << getpid();
#endif
    move(wind_y + 1, 0);

    attron(A_REVERSE | A_STANDOUT);
    printw(oss.str().c_str());
    standend();


    refresh();
}

bool Display::is_printable(char c){
    return (c != '\r');
}

int Display::get_display_width(){
    return wind_x;
}

void Display::set_menu(Menu new_menu){
    items = new_menu;
    selected_item = 0;
    region_start = 0;
}

void Display::jump_to(pair<unsigned int, unsigned int> indices){
    last_selected_item = selected_item;
    last_region_start = region_start;
    selected_item = indices.first;
    region_start = indices.second;
}

void Display::move_cursor(int amount){

    int temp_sel_item = (int) selected_item + amount;
    if(temp_sel_item < 0){
        selected_item = 0;
    }else if(temp_sel_item >= (int) items.size()){
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
