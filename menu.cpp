#include "menu.h"

#include <stdexcept>
#include <iostream>

using std::vector;
using std::string;
using std::runtime_error;

MenuItem Menu::no_item = MenuItem(true);

Menu::Menu(){
}

Menu::Menu(const string& gopher_doc){
    vector<string> lines = get_lines(gopher_doc);
    for(unsigned int i = 0; i < lines.size(); ++i){
        items.emplace_back(lines.at(i));
    }
}

Menu::Menu(const string& gopher_doc, MenuItem doc_item){
    vector<string> lines = get_lines(gopher_doc);
    for(unsigned int i = 0; i < lines.size(); ++i){

        switch(doc_item.get_type()){
        case '0':
        {
            MenuItem newItem;
            newItem.item_type = 'i';
            newItem.item_text = lines.at(i);
            newItem.selector_string = "";
            items.push_back(newItem);
        }
            break;
        case '1':
            items.emplace_back(lines.at(i));
            break;
        default:
            break;
        }

    }
}

void Menu::print_items(){
    for(unsigned int i = 0; i < items.size(); ++i){
        std::cout << items.at(i).get_text();
        if(items.at(i).get_type() != 'i'){
            std::cout << "\t" << items.at(i).get_selector();
        }
        std::cout << std::endl;
    }
}

vector<string> Menu::get_lines(const string& gopher_doc){
    bool finding_items = true;
    size_t last_pos = 0;
    vector<string> lines;

    const string DELIMITER("\r\n");
    while(finding_items){
        size_t new_pos = gopher_doc.find(DELIMITER, last_pos);
        if(new_pos == string::npos){
            finding_items = false;
            continue;
        }

        string itemstring = gopher_doc.substr(last_pos, new_pos - last_pos);
        if(itemstring.size() == 1 && itemstring[0] == '.'){
            finding_items = false;
            continue;
        }
        last_pos = new_pos + DELIMITER.length();
        lines.push_back(itemstring);
    }
    return lines;
}

size_t Menu::size(){
    return items.size();
}

MenuItem& Menu::at(size_t pos){
    return items.at(pos);
}

MenuItem::MenuItem(const string& gopher_line){
    const string DELIMITER("\t");
    isEmpty = false;

    size_t firsttab = gopher_line.find(DELIMITER);
    if(firsttab == string::npos){
        throw runtime_error("Couldn't parse menu item (no tabs)");
    }

    size_t secondtab = gopher_line.find(DELIMITER, firsttab + 1);
    if(secondtab == string::npos){
        throw runtime_error("Couldn't parse menu item (need at least 2 tabs)");
    }

    item_type = gopher_line.at(0);
    item_text = gopher_line.substr(1, firsttab - 1);
    selector_string = gopher_line.substr(firsttab + 1);
}

bool MenuItem::operator==(const MenuItem other) const{
    if(other.isEmpty && isEmpty){
        return true;
    }
    return other.item_type == item_type &&
        other.item_text == item_text &&
        other.selector_string == selector_string;
}

bool MenuItem::operator!=(const MenuItem other) const{
    return !(*this == other);
}
MenuItem MenuItem::operator=(const MenuItem other){
    item_type = other.item_type;
    item_text = other.item_text;
    selector_string = other.selector_string;
    isEmpty = other.isEmpty;
    return other;
}

string MenuItem::get_selector() const{
    return selector_string;
}

string MenuItem::get_text() const {
    return item_text;
}

char MenuItem::get_type() const{
    return item_type;
}


MenuItem::MenuItem(){
    isEmpty = false;
}

MenuItem::MenuItem(bool empty){
    isEmpty = empty;
}
