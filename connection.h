#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <string>

class Connection{
public:
    Connection(const std::string& host);
    Connection();
    ~Connection();
    void set_host(const std::string& host);
    void open();
    void close_conn();
    std::string request(const std::string& request);
private:
    std::string get_response();
    bool send_str(const std::string& request);
    std::string hostname;
    std::string port;
    int sockFD;
    bool connected;
};

#endif
