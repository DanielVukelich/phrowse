#ifndef _GURI_H
#define _GURI_H

#include <string>

class GURI{
public:
    GURI();
    GURI(const std::string& uri);
    void set_uri(const std::string& uri);
    std::string get_formatted_host(char delimiter) const;
    std::string get_host() const;
    std::string get_port() const;
    std::string get_resource() const;
private:
    std::string hostname;
    std::string port;
    std::string resource;
};

#endif
