#include "connection.h"
#include "menu.h"
#include "display.h"
#include "history.h"
#include "guri.h"

#include <iostream>
#include <string>
#include <stdint.h>
#include <stdexcept>

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::runtime_error;

void print_usage(char* argv0){
    cout << "Usage: " << argv0 << " [gopher://]<domain>[:port][/resource]" << endl;
    return;
}

void get_args(int argc, char** argv, GURI& uri, Display& disp){
    if(argc == 2){
        string arg = string(argv[1]);
        if(!arg.compare("-h") || !arg.compare("--help")){
            disp.~Display();
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        }
        try{
            uri.set_uri(arg);
            return;
        }catch(runtime_error e){
            disp.~Display();
            cout << e.what() << endl;
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    disp.~Display();
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){

    Display disp;
    Menu::set_display_width(disp.get_display_width());
    disp.print_prompt();


    GURI uri;
    get_args(argc, argv, uri, disp);

    string host = uri.get_formatted_host('\t');
    Connection conn(host);

    string query = uri.get_resource();

    string first_page_item = "1\t";
    first_page_item.append(query);
    first_page_item.append("\t");
    first_page_item.append(host);
    query.append("\r\n");

    Menu men;
    try{
        men = Menu(conn.request(query));
    }catch(runtime_error e){
        men = Menu::from_exception(e.what());
    }

    disp.set_menu(men);
    disp.draw_menu();

    MenuItem firstpage(first_page_item);

    BrowserHistory hist;
    hist.add_item(firstpage);
    HistoryItem to_visit;

    while((to_visit = HistoryItem(disp.get_item())).get_item() != Menu::no_item){

        if(to_visit.get_item() == Menu::prev_item){
            to_visit = hist.go_back();
            hist.set_fut_indices(disp.get_last_sel());
        }else if(to_visit.get_item() == Menu::next_item){
            to_visit = hist.go_forward();
        }else{
            hist.clear_future();
            hist.set_hist_indices(disp.get_last_sel());
        }

        //If the user is trying to go fwd/back but there is no history,
        //do nothing
        if(to_visit.get_item() == Menu::no_item){
            continue;
        }

        try{
            string response = conn.request(to_visit.get_item());
            men = Menu(response, to_visit.get_item().get_type());
        }catch(runtime_error e){
            men = Menu::from_exception(e.what());
        }

        hist.add_item(to_visit);

        disp.set_menu(men);
        disp.jump_to(to_visit.get_indices());
        disp.draw_menu();
    }

    return 0;
}
