#include "connection.h"
#include "menu.h"
#include "display.h"

#include <iostream>
#include <string>
#include <stdint.h>
#include <stdexcept>
#include <unistd.h>
#include <stack>

using std::stack;
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
    string query;
    disp.print_prompt();
    string host;
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
    MenuItem item;

    bool last_item_was_fut = false;
    MenuItem firstpage(first_page_item);
    stack<MenuItem> history;
    history.push(firstpage);
    stack<MenuItem> future;

    while((item = disp.get_item()) != Menu::no_item){
        if(item == Menu::no_item){
            break;
        }else if(item == Menu::prev_item){
            if(history.size() < 2){
                continue;
            }
            last_item_was_fut = false;
            item = history.top();
            history.pop();
            future.push(item);
            item = history.top();
            history.pop();
        }else if(item == Menu::next_item){
            if(!future.size()){
                continue;
            }
            last_item_was_fut = true;
            item = future.top();
            future.pop();
            history.push(item);
        }else{
            last_item_was_fut = false;
            future = stack<MenuItem>();
        }
        string response = conn.request(item);

        men = Menu(response, item.get_type());
        if(!last_item_was_fut){
            history.push(item);
        }
        disp.set_menu(men);
        disp.draw_menu();
    }

    return 0;
}
