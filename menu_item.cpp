#include "menu_item.h"

#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

using std::runtime_error;
using std::ostringstream;
using std::stringstream;
using std::string;
using std::getline;

MenuItem::MenuItem(){
    attr = special_type::NORMAL;
}

MenuItem::MenuItem(special_type type){
    attr = type;
}

MenuItem::MenuItem(const GURI& url){
    ostringstream oss;
    oss << "1\t" << url.get_resource()
        << "\t" << url.get_formatted_host('\t');
    init_by_line(oss.str());
}

MenuItem::MenuItem(const string& gopher_line){
    init_by_line(gopher_line);
}

MenuItem::MenuItem(char type, const string& gopher_line){
    init_by_type(type, gopher_line);
}

void MenuItem::init_by_line(const string& gopher_line){
    const string DELIMITER("\t");
    attr = special_type::NORMAL;

    if(!gopher_line.size()){
        item_type = 'i';
        item_text = "";
        selector_string = "NULL\tNULL";
        return;
    }

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

bool MenuItem::can_select() const{
    switch(item_type){
    case '3':
    case 'i':
        return false;
    case '0':
    case '1':
    case '2':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '+':
    case 'T':
    case 'g':
    case 'I':
    case 'h':
    case 'p':
    default:
        return true;
    }
}


bool MenuItem::is_binary() const{
    switch(item_type){
    case '4':
    case '5':
    case '6':
    case '9':
    case 'g':
    case 'I':
    case 'h':
    case 'p':
        return true;
    case '3':
    case 'i':
    case '0':
    case '1':
    case '2':
    case '7':
    case '8':
    case '+':
    case 'T':
    default:
        return false;
    }
}

void MenuItem::build_request(string& req, string& host) const{
    string port;
    stringstream ss(selector_string);
    getline(ss, req, '\t');
    getline(ss, host, '\t');
    getline(ss, port, '\t');
    host.push_back('\t');
    host.append(port);
    switch(item_type){
    case '0':
    case '1':
    case '3':
    case '4':
    case '5':
    case '6':
    case '9':
    case '+':
    case 'g':
    case 'I':
    case 'p':
        break;
    case '7':
        req.push_back('\t');
        req.append(search);
        break;
    default:
        ostringstream err;
        err << "Menu item type " << describe_item() << " not supported";
        throw runtime_error(err.str());
    }
    req.append("\r\n");
}

void MenuItem::init_by_type(char type, const string& gopher_line){
    item_type = type;
    switch(type){
    case '0':
        item_type = 'i';
        item_text = gopher_line;
        selector_string = "";
        break;
    case '7':
    case '3':
    case '1':
        init_by_line(gopher_line);
        break;
    default:
        ostringstream oss;
        oss << "Couldn't load menu for document type \"" << type << "\"";
        throw runtime_error(oss.str());
        break;
    }
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
    case 'h':
        return string("HTTP link");
    case 'p':
        return string("PNG image");
    default:
        string s("Unknown item: ");
        s.push_back(item_type);
        return s;
    }
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

void MenuItem::set_search(const std::string& query){
    search = query;
}

string MenuItem::get_search() const{
    return search;
}

int MenuItem::get_line_width(char type, int scrn_width){
    switch(type){
    case '0':
        return scrn_width;
    case '7':
    case '3':
    case '1':
    default:
        return -1;
    }
}

string MenuItem::get_terminator(char type){
    switch(type){
    case '0':
        return string("");
    case '7':
    case '3':
    case '1':
    default:
        return string(".");
    }
}

string MenuItem::get_delimiter(char type){
    switch(type){
    case '0':
        return string("\n");
    case '7':
    case '3':
    case '1':
    default:
        return string("\r\n");
    }
}
