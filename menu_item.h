#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H

#include "guri.h"

#include <string>

class MenuItem{
    friend class Menu;
public:
    MenuItem();
    MenuItem(const GURI& url);
    MenuItem(const std::string& gopher_line);
    MenuItem(char type, const std::string& gopher_line);
    std::string get_text() const;
    std::string get_selector() const;
    std::string describe_item() const;
    void set_search(const std::string& query);
    std::string get_search() const;
    char get_type() const;
    bool is_binary() const;
    bool can_select() const;
    bool is_supported_type() const;
    void build_request(std::string& req, std::string& host) const;
    bool operator==(const MenuItem other) const;
    bool operator!=(const MenuItem other) const;
    MenuItem operator=(const MenuItem other);
    static std::string get_delimiter(char type);
    static std::string get_terminator(char type);
    static int get_line_width(char type, int scrn_width);
private:
    enum special_type{
        NO_ITEM,
        PREV_ITEM,
        NEXT_ITEM,
        URL_ITEM,
        NORMAL
    };
    MenuItem(special_type type);
    void init_by_type(char type, const std::string& gopher_line);
    void init_by_line(const std::string& gopher_line);
    char item_type;
    std::string item_text;
    std::string selector_string;
    std::string search;
    special_type attr;
};

#endif
