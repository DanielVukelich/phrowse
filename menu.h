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
    std::string describe_item() const;
    char get_type() const;
    bool operator==(const MenuItem other) const;
    bool operator!=(const MenuItem other) const;
    MenuItem operator=(const MenuItem other);
private:
    enum special_type{
        NO_ITEM,
        PREV_ITEM,
        NEXT_ITEM,
        NORMAL
    };
    MenuItem(special_type type);
    char item_type;
    std::string item_text;
    std::string selector_string;
    special_type attr;
};

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
