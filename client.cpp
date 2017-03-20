#include "connection.h"

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
    cout << "Usage: " << argv0 << " <hostname:port>" << endl;
    return;
}

int main(int argc, char** argv){
    string host;
    if(argc == 2){
        host = string(argv[1]);
        if(!host.compare("-h") || !host.compare("--help")){
            print_usage(argv[0]);
            return 0;
        }
    }else if(argc > 2){
        print_usage(argv[0]);
        return 0;
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
    Connection conn;
    conn.set_host(host);

    string query;
    cout << ">";
    while(std::getline(cin, query)){
        query.append("\r\n");
        try{
            conn.open();
        }catch(runtime_error e){
            cout << "Error: " << e.what() << endl;
            continue;
        }
        cout << conn.request(query) << endl;
        conn.close_conn();
        cout << ">";
    }
    cout << endl;
    return 0;
}
