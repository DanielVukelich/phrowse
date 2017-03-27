#ifndef _MENU_H
#define _MENU_H

#include <string>
#include <vector>

class MenuItem{
public:
    MenuItem(const std::string& gopher_line);
    std::string get_text();
    std::string get_selector();
    char get_type();
private:
    char item_type;
    std::string item_text;
    std::string selector_string;
};

class Menu{
public:
    Menu(const std::string& gopher_doc);
    Menu();
    size_t size();
    MenuItem& at(size_t pos);
    void print_items();
private:
    std::vector<MenuItem> items;
};

#endif
