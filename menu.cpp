#include "menu.h"

#include <stdexcept>
#include <iostream>

using std::vector;
using std::string;
using std::runtime_error;

MenuItem Menu::no_item = MenuItem(MenuItem::special_type::NO_ITEM);
MenuItem Menu::prev_item = MenuItem(MenuItem::special_type::PREV_ITEM);
MenuItem Menu::next_item = MenuItem(MenuItem::special_type::NEXT_ITEM);

Menu::Menu(){
}

Menu::Menu(const string& gopher_doc, char doc_type){
    vector<string> lines;
    try{
        lines = get_lines(gopher_doc, doc_type);
        if(!lines.size() && gopher_doc.size()){
            throw runtime_error("Couldn't parse lines.  Trying in text mode");
        }
    }catch(runtime_error e){
        doc_type = '0';
        lines = get_lines(gopher_doc, doc_type);
    }

    for(unsigned int i = 0; i < lines.size(); ++i){

        switch(doc_type){
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

vector<string> Menu::get_lines(const string& gopher_doc,
                               char doc_type){
    string delimiter, terminator;
    switch(doc_type){
    case '0':
        delimiter = "\n";
        terminator = "";
        break;
    case '1':
    default:
        delimiter = "\r\n";
        terminator = ".";
        break;
    }
    return get_lines(gopher_doc, delimiter, terminator);
}

vector<string> Menu::get_lines(const string& gopher_doc,
                               const string& delimiter,
                               const string& terminator){
    bool finding_items = true;
    size_t last_pos = 0;
    vector<string> lines;

    while(finding_items){
        size_t new_pos = gopher_doc.find(delimiter, last_pos);
        if(new_pos == string::npos){
            finding_items = false;
            continue;
        }

        string itemstring = gopher_doc.substr(last_pos, new_pos - last_pos);
        if(terminator.size() && itemstring == terminator){
            finding_items = false;
            continue;
        }
        last_pos = new_pos + delimiter.length();
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

string MenuItem::describe_item() const{
    switch(item_type){
    case '0':
        return string("File");
    case '1':
        return string("Gopher menu");
    case '2':
        return string("CSO server");
    case '3':
        return string("Error message");
    case '4':
        return string("Macintosh file");
    case '5':
        return string("DOS binary archive");
    case '6':
        return string("UNIX uuencoded file");
    case '7':
        return string("Index-Search server");
    case '8':
        return string("Telnet session");
    case '9':
        return string("Binary file");
    case '+':
        return string("Redundant server");
    case 'T':
        return string("tn3270 session");
    case 'g':
        return string("GIF image");
    case 'i':
        return string("Text/Information");
    case 'I':
        return string("Image");
    default:
        return string("Unknown item");
    }
}

MenuItem::MenuItem(const string& gopher_line){
    const string DELIMITER("\t");
    attr = special_type::NORMAL;

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

    if(attr != special_type::NORMAL){
        return (attr == other.attr);
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
    attr = other.attr;
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
    attr = special_type::NORMAL;
}

MenuItem::MenuItem(special_type type){
    attr = type;
}
