#ifndef _MENU_H
#define _MENU_H

#include "menu_item.h"

#include <string>
#include <vector>

class Menu{
public:
    Menu(const std::string& gopher_doc, char doc_type);
    Menu(const std::string& gopher_doc) : Menu(gopher_doc, '1'){}
    static Menu from_exception(const std::string& what);
    Menu();
    static void set_display_width(int width);
    size_t size();
    MenuItem& at(size_t pos);
    void print_items();
    static MenuItem no_item;
    static MenuItem prev_item;
    static MenuItem next_item;
    static MenuItem url_item;
private:
    static int display_width;
    std::vector<MenuItem> items;
    std::vector<std::string> get_lines(const std::string& gopher_doc,
                                       const std::string& delimiter,
                                       const std::string& terminator,
                                       int max_line_len);
    std::vector<std::string> get_lines(const std::string& gopher_doc,
                                       char doc_type);
};

#endif
