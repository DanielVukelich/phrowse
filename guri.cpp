#include "guri.h"

#include <stdexcept>
#include <cstdlib>
#include <sstream>

using std::runtime_error;
using std::string;
using std::ostringstream;

GURI::GURI(){
}

GURI::GURI(const string& uri){
    set_uri(uri);
}

void GURI::set_uri(const string& uri){
    const string PROTO_END = "://";
    const string PROTO = "gopher";
    const string COLON = ":";

    port = "70";
    resource = "/";
    hostname = "";

    size_t proto_end_loc = uri.find(PROTO_END);
    size_t domain_begin = 0;
    if(proto_end_loc != string::npos){
        string uri_proto = uri.substr(0, proto_end_loc);
        if(uri_proto != PROTO){
            throw runtime_error("URL was not valid for gopher");
        }
        domain_begin = proto_end_loc + PROTO_END.size();
    }
    size_t port_begin_loc = uri.find(COLON, domain_begin);
    size_t resource_begin_loc = uri.find("/", domain_begin);

    size_t domain_end = uri.size();
    if(port_begin_loc < resource_begin_loc){
        domain_end = port_begin_loc;
    }else if(resource_begin_loc < port_begin_loc){
        domain_end = resource_begin_loc;
    }
    hostname = uri.substr(domain_begin, domain_end - domain_begin);

    if(port_begin_loc != string::npos){
        port = uri.substr(port_begin_loc + COLON.size(),
                          (resource_begin_loc - 1) - port_begin_loc);
    }

    if(resource_begin_loc != string::npos){
        resource = uri.substr(resource_begin_loc);
    }

    size_t hostname_colon = hostname.find(COLON);
    size_t port_colon = port.find(COLON);
    size_t resource_colon = resource.find(COLON);
    if(hostname_colon != string::npos ||
       port_colon != string::npos ||
       resource_colon != string::npos){
        throw runtime_error("URL was not valid for gopher");
    }

    int portnum = atoi(port.c_str());
    if(portnum == 0 || portnum > 65535){
        throw runtime_error("URL was not valid for gopher (port number)");
    }

}

string GURI::get_host(){
    return hostname;
}

string GURI::get_port(){
    return port;
}

string GURI::get_resource(){
    return resource;
}

string GURI::get_formatted_host(char delimiter){
    ostringstream oss;
    oss << hostname << delimiter << port;
    return oss.str();
}
