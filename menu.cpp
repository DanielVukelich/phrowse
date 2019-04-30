#include "menu.h"

#include <sstream>
#include <stdexcept>

using std::ostringstream;
using std::vector;
using std::string;
using std::runtime_error;

MenuItem Menu::no_item = MenuItem(MenuItem::special_type::NO_ITEM);
MenuItem Menu::prev_item = MenuItem(MenuItem::special_type::PREV_ITEM);
MenuItem Menu::next_item = MenuItem(MenuItem::special_type::NEXT_ITEM);
MenuItem Menu::url_item = MenuItem(MenuItem::special_type::URL_ITEM);
int Menu::display_width = 40;

Menu::Menu(){
}

Menu::Menu(const std::exception& e){
    ostringstream ss;
    ss << "3Error while loading page:\tNULL\tNULL\r\n"
       << "3" << e.what() << "\tNULL\tNULL\r\n"
       << ".\r\n";
    parse_lines(ss.str(), '3');
}

Menu::Menu(const string& gopher_doc, char doc_type){
    parse_lines(gopher_doc, doc_type);
}

void Menu::parse_lines(const string& doc, char doc_type){
    vector<string> lines;
    try{
        lines = get_lines(doc, doc_type);
        if(!lines.size() && doc.size()){
            throw runtime_error("Couldn't parse lines.  Trying in text mode");
        }
    }catch(runtime_error &e){
        doc_type = '0';
        lines = get_lines(doc, doc_type);
    }

    for(unsigned int i = 0; i < lines.size(); ++i){
        items.emplace_back(doc_type, lines.at(i));
    }
}

void Menu::set_display_width(int width){
    display_width = width;
}

vector<string> Menu::get_lines(const string& gopher_doc,
                               char doc_type){
    string delimiter = MenuItem::get_delimiter(doc_type);
    string terminator = MenuItem::get_terminator(doc_type);
    int width = MenuItem::get_line_width(doc_type, display_width);
    return get_lines(gopher_doc, delimiter, terminator, width);
}

vector<string> Menu::get_lines(const string& gopher_doc,
                               const string& delimiter,
                               const string& terminator,
                               int max_line_len){
    bool finding_items = true;
    size_t last_pos = 0;
    vector<string> lines;

    while(finding_items){
        size_t new_pos = gopher_doc.find(delimiter, last_pos);
        if(new_pos == string::npos){
            finding_items = false;
            continue;
        }

        bool truncated_line = false;
        if((max_line_len > 0) && ((new_pos - last_pos) > (unsigned int) max_line_len)){
            new_pos = last_pos + max_line_len;
            truncated_line = true;
        }

        string itemstring = gopher_doc.substr(last_pos, new_pos - last_pos);
        if(terminator.size() && itemstring == terminator){
            finding_items = false;
            continue;
        }

        last_pos = new_pos;
        if(!truncated_line){
            last_pos += delimiter.length();
        }
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
