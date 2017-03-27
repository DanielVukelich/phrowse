#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <string>
#include <unordered_map>

class Connection{
public:
    Connection(const std::string& host);
    Connection();
    ~Connection();
    void set_host(const std::string& host);
    std::string request(const std::string& request, const std::string& host);
    std::string request(const std::string& request);
private:
    void open();
    void close_conn();
    std::string get_response();
    bool send_str(const std::string& request);
    std::string hostname;
    std::string port;
    int sockFD;
    bool connected;
    std::unordered_map<std::string, std::string> cache;
};

#endif
