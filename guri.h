#ifndef _GURI_H
#define _GURI_H

#include <string>

class GURI{
public:
    GURI();
    GURI(const std::string& uri);
    void set_uri(const std::string& uri);
    std::string get_formatted_host(char delimiter);
    std::string get_host();
    std::string get_port();
    std::string get_resource();
private:
    std::string hostname;
    std::string port;
    std::string resource;
};

#endif
