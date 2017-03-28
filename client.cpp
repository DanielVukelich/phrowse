#include "connection.h"
#include "menu.h"
#include "display.h"
#include "history.h"

#include <iostream>
#include <string>
#include <stdint.h>
#include <stdexcept>
#include <unistd.h>

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::runtime_error;

void print_usage(char* argv0){
    cout << "Usage: " << argv0 << " <hostname:port>" << endl;
    return;
}

void get_args(int argc, char** argv, string& host, Display& disp){
    host = "";
    if(argc == 2){
        host = string(argv[1]);
        if(!host.compare("-h") || !host.compare("--help")){
            disp.~Display();
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        }
    }else if(argc > 2){
#ifdef DEBUG
        //If a debug build is launched with commandline arg 'g', then tell the user
        //what the pid is and wait until it's been attached
        if(argc == 3 && argv[2][0] == 'g' && argv[2][1] == '\0'){
            char await;
            host = string(argv[1]);
            std::cout << "Attach debugger to pid " << getpid() << "\nInput any character to continue";
            std::cin >> await;
        }else{
#endif
            disp.~Display();
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
#ifdef DEBUG
        }
#endif
    }else{
        cout << "Enter <hostname:port>.  Port defaults to 70."
             << endl << ">";
        cin >> host;
    }

    size_t loc = host.find(":");
    if(loc != string::npos){
        host.at(loc) = '\t';
    }else{
        host.append("\t70");
    }
    return;
}

int main(int argc, char** argv){

    Display disp;
    Menu::set_display_width(disp.get_display_width());
    disp.print_prompt();

    string host, query;
    get_args(argc, argv, host, disp);
    Connection conn(host);

    disp.get_line(query);
    if(!query.size()){
        query.append("/");
    }

    string first_page_item = "1\t";
    first_page_item.append(query);
    first_page_item.append("\t");
    first_page_item.append(host);

    query.append("\r\n");
    Menu men(conn.request(query));

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

        string response = conn.request(to_visit.get_item());

        men = Menu(response, to_visit.get_item().get_type());

        hist.add_item(to_visit);

        disp.set_menu(men);
        disp.jump_to(to_visit.get_indices());
        disp.draw_menu();
    }

    return 0;
}
