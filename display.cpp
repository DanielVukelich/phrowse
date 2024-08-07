#include "display.h"

#include <ncurses.h>
#include <string>
#include <sstream>
#include <unistd.h>

#define BACKSPACE 127
#define CTRLD 4
#define CTRLC 3

using std::ostringstream;
using std::string;
using std::pair;
using std::make_pair;
using std::vector;

Display::Display(){
    initscr();
    getmaxyx(stdscr, wind_y, wind_x);
    raw();
    noecho();
    keypad(stdscr, true);
    selected_item = 0;
    region_start = 0;
}

Display::~Display(){
    endwin();
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
            getting_input = !items.at(selected_item).can_select();
            break;
        case 'g':
            return Menu::url_item;
        case 'i':
            move_cursor(next_selectable_dist(false));
            break;
        case 'k':
            move_cursor(next_selectable_dist(true));
            break;
        case CTRLD:
        case CTRLC:
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

void Display::print_text(const string& text){
    clear();
    printw("%s", text.c_str());
    refresh();
}

pair<unsigned int, unsigned int> Display::get_last_sel(){
    return make_pair(last_selected_item, last_region_start);
}

bool Display::get_string(const std::string& prompt, string& result){
    return get_string(prompt.c_str(), result);
}

bool Display::get_string(const char* prompt, string& result){
    clear();
    attron(A_REVERSE);
    printw("%s", prompt);
    standend();
    addch('\n');
    refresh();

    result.clear();

    bool get_input = true;
    while(get_input){
        char key = getch();
        if(key >= ' ' && key <= '~'){
            addch(key);
            refresh();
            result.push_back(key);
        }else if(key == BACKSPACE){
            if(result.size()){
                printw("\b \b");
                refresh();
                result.pop_back();
            }
        }else if(key == CTRLD || key == CTRLC){
            return false;
        }else if (key == '\n'){
            get_input = false;
        }
    }
    return true;
}

void Display::draw_menu(){
    clear();

    if(selected_item >= items.size()){
        last_selected_item = selected_item;
        last_region_start = region_start;
        selected_item = 0;
        region_start = 0;
    }

    for(unsigned int i = region_start, j = 0;
        i < items.size() && j != (unsigned int) wind_y;
        ++i, ++j){

        string it = items.at(i).get_text();


        if(selected_item == i){
            attron(A_REVERSE);
            if(items.at(i).can_select())
                attron(A_UNDERLINE);
        }

        vector<char> print_buffer;
        //Reserve length + 2 for the newline and null terminator
        print_buffer.reserve(it.length() + 2);
        for(unsigned int k = 0; k < it.length(); ++k){
            char c = it.at(k);
            if(is_printable(c)){
                escape(print_buffer, c);
            }
        }

        if(!print_buffer.size())
            print_buffer.push_back(' ');
        print_buffer.push_back('\n');
        print_buffer.push_back('\0');

        printw("%s", &print_buffer[0]);
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
    printw("%s", oss.str().c_str());
    standend();


    refresh();
}

void Display::escape(vector<char>& buff, char c){
    if(c == '%')
        buff.push_back('%');
    buff.push_back(c);
}

bool Display::is_printable(char c){
    return (c == '\n' || (c >= ' ' && c <= '~'));
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
    if(!amount)
        return;

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

int Display::next_selectable_dist(bool down){

    bool searching = true;
    bool firstloop = true;
    unsigned int i = selected_item;
    unsigned int foundpos = selected_item;

    while(searching){
        if(items.at(i).can_select() && !firstloop){
            foundpos = i;
            break;
        }

        if(down){
            if(i + 1 < items.size())
                ++i;
            else
                break;
        }else{
            if(i)
                --i;
            else
                break;
        }

        firstloop = false;
    }
    return ((int)foundpos) - ((int)selected_item);
}
