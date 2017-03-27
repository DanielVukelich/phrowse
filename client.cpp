#include "connection.h"
#include "menu.h"
#include "display.h"

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
    string query;
    disp.print_prompt();
    string host;
    get_args(argc, argv, host, disp);
    Connection conn(host);

    while(disp.get_line(query)){

        query.append("\r\n");
        Menu men(conn.request(query));
        disp.set_menu(men);
        disp.draw_menu();
        MenuItem it = disp.get_item();
        if(it == Menu::no_item){
            break;
        }
        disp.print_prompt();
    }

    return 0;
}
