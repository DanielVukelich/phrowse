#ifndef _MENU_H
#define _MENU_H

#include <string>
#include <vector>

class MenuItem{
    friend class Menu;
public:
    MenuItem(const std::string& gopher_line);
    std::string get_text();
    std::string get_selector();
    char get_type();
    bool operator==(const MenuItem other);
private:
    MenuItem(bool empty);
    char item_type;
    std::string item_text;
    std::string selector_string;
    bool isEmpty;
};

class Menu{
public:
    Menu(const std::string& gopher_doc);
    Menu();
    size_t size();
    MenuItem& at(size_t pos);
    void print_items();
    static MenuItem no_item;
private:
    std::vector<MenuItem> items;
};

#endif
