#ifndef _MENU_H
#define _MENU_H

#include <string>
#include <vector>

class MenuItem{
    friend class Menu;
public:
    MenuItem(const std::string& gopher_line);
    MenuItem();
    std::string get_text() const;
    std::string get_selector() const;
    char get_type() const;
    bool operator==(const MenuItem other) const;
    bool operator!=(const MenuItem other) const;
    MenuItem operator=(const MenuItem other);
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
    Menu(const std::string& gopher_doc, MenuItem doc_item);
    Menu();
    size_t size();
    MenuItem& at(size_t pos);
    void print_items();
    static MenuItem no_item;
private:
    std::vector<MenuItem> items;
    std::vector<std::string> get_lines(const std::string& gopher_doc);
};

#endif
