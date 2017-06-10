#include "connection.h"
#include "menu.h"
#include "display.h"
#include "history.h"
#include "menu_item.h"
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

    GURI uri;
    get_args(argc, argv, uri, disp);

    disp.print_text("Welcome to Phrowse!\n");

    Connection conn(uri.get_formatted_host('\t'));
    MenuItem firstpage(uri);

    Menu men;
    try{
        men = Menu(conn.request(firstpage));
    }catch(runtime_error e){
        men = Menu(e);
    }

    disp.set_menu(men);
    disp.draw_menu();


    BrowserHistory hist;
    hist.add_item(firstpage);
    HistoryItem to_visit;

    while((to_visit = HistoryItem(disp.get_item())).get_item() != Menu::no_item){

        if(!to_visit.get_item().can_select()){
            continue;
        }

        if(to_visit.get_item() == Menu::prev_item){
            to_visit = hist.go_back();
            hist.set_fut_indices(disp.get_last_sel());
        }else if(to_visit.get_item() == Menu::next_item){
            to_visit = hist.go_forward();
        }else if(to_visit.get_item() == Menu::url_item){
            hist.clear_future();
            hist.set_hist_indices(disp.get_last_sel());
            string url = "";
            string prompt = "Enter a gopher URL to visit:";
            while(true){
                try{
                    if(!disp.get_string(prompt, url)){
                        throw runtime_error("Please enter a URL");
                    }
                    GURI new_page(url);
                    to_visit.get_item() = MenuItem(new_page);
                    break;
                }catch(runtime_error e){
                    prompt = e.what();
                    prompt.append("\nEnter a gopher URL to visit:");
                }
            }
        }else{
            hist.clear_future();
            hist.set_hist_indices(disp.get_last_sel());

            if(to_visit.get_item().get_type() == '7'){
                string query;
                if(!disp.get_string("Enter a search query:", query)){
                    break;
                }
                to_visit.get_item().set_search(query);
            }
        }

        //If the user is trying to go fwd/back but there is no history,
        //do nothing
        if(to_visit.get_item() == Menu::no_item){
            continue;
        }

        try{
            string response = conn.request(to_visit.get_item());
            if(to_visit.get_item().is_binary()){
                men = Menu(response, '1');
            }else{
                men = Menu(response, to_visit.get_item().get_type());
            }
        }catch(runtime_error e){
            men = Menu(e);
        }

        hist.add_item(to_visit);

        disp.set_menu(men);
        disp.jump_to(to_visit.get_indices());
        disp.draw_menu();
    }

    return 0;
}
