#include "connection.h"

#include <errno.h>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <stdexcept>
#include <netdb.h>
#include <sys/socket.h>
#include <fcntl.h>

const unsigned int TIMEOUT_SECONDS = 10;
const unsigned int TIMEOUT_USECONDS = 0;
const unsigned int BUFFER_SIZE = 1024;
const unsigned int FAST_RETRIES = 10;
const unsigned int SLOW_RETRIES = 5;
const unsigned int SLEEP_USECONDS = 100000;
const char* TERMINATING = "\r\n.\r\n";

using std::ostringstream;
using std::stringstream;
using std::string;
using std::runtime_error;

Connection::Connection(){
    sockFD = 0;
    connected = false;
}

Connection::Connection(const string& host){
    sockFD = 0;
    connected = false;
    set_host(host);
}

Connection::~Connection(){
    close_conn();
}

void Connection::close_conn(){
    if(connected){
        close(sockFD);
    }
    connected = false;
}

void Connection::set_host(const string& host){
    unsigned int pos = host.find('\t');
    if(pos == string::npos || pos == host.size() - 1){
        hostname = host;
        port = "70";
        if(pos == host.size() - 1){
            hostname.pop_back();
        }
    }else{
        hostname = host.substr(0, pos);
        port = host.substr(pos + 1);
    }
}

string Connection::request(const MenuItem& item){
    string req, host, port;
    stringstream ss(item.get_selector());
    switch(item.get_type()){
    case '0':
    case '1':
        getline(ss, req, '\t');
        getline(ss, host, '\t');
        getline(ss, port, '\t');
        host.push_back('\t');
        host.append(port);
        set_host(host);
        break;
    case '7':
        getline(ss, req, '\t');
        getline(ss, host, '\t');
        getline(ss, port, '\t');
        host.push_back('\t');
        host.append(port);
        set_host(host);
        req.push_back('\t');
        req.append(item.get_search());
        break;
    default:
        return string();
    }
    req.append("\r\n");
    return request(req);
}

string Connection::request(const string& request, const string& host){
    set_host(host);
    return this->request(request);
}

string Connection::request(const string& request){
    string cache_key = hostname + port + request;
    auto it = cache.find(cache_key);

    if(it != cache.end()){
        return it->second;
    }

    open();
    if(send_str(request)){
        it = cache.emplace(cache_key, get_response()).first;
        close_conn();
        return it->second;
    }
    close_conn();
    return string();
}

string Connection::get_response(){
    if(!connected){
        throw runtime_error("Not connected to server!");
    }

    char* buf = new char[BUFFER_SIZE];
    bool receiving = true;
    string response;
    unsigned int consec_empty = 0;

    while(receiving){
        ssize_t rec = recv(sockFD, (void*) buf, BUFFER_SIZE, 0);
        if(rec == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                receiving = false;
            }else if(errno == ENOTCONN || errno == ECONNRESET){
                close(sockFD);
                connected = false;
                receiving = false;
            }else{
                throw runtime_error(strerror(errno));
            }
        }else{
            if(!rec){

                receiving =
                    (response.find(TERMINATING,
                                  response.size() - strlen(TERMINATING) + 2)
                    == string::npos);

                if(consec_empty > FAST_RETRIES){
                    if(consec_empty > FAST_RETRIES + SLOW_RETRIES){
                        receiving = false;
                        continue;
                    }
                    usleep(SLEEP_USECONDS);
                }
                ++consec_empty;
            }else{
                consec_empty = 0;
                response.append(buf, rec);
            }
        }
    }
    delete[] buf;
    return response;
}

bool Connection::send_str(const std::string& request){
    if(!connected){
        throw runtime_error("Not connected to server!");
    }
    unsigned int sent = 0;
    while(sent != request.size()){
        int res = send(sockFD, (void*) (request.data() + sent), request.size() - sent, 0);
        if(res == -1){
            if(errno == ENOTCONN || errno == ECONNRESET){
                close(sockFD);
                connected = false;
                return false;
            }else{
                throw runtime_error(strerror(errno));
            }
        }
        sent += res;
    }
    return true;
}

void Connection::open(){

    ostringstream ss;

    struct timeval  timeout;
    timeout.tv_sec = TIMEOUT_SECONDS;
    timeout.tv_usec = TIMEOUT_USECONDS;

    addrinfo hints;
    addrinfo *servInfo;
    addrinfo *ptr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (auto error =
            getaddrinfo(hostname.c_str(), port.c_str(), &hints, &servInfo)) {
        ss << "Couldn't find " << hostname << ": " << gai_strerror(error);
        throw runtime_error(ss.str());
    }

    // Iterate through available addresses matching hints
    for (ptr = servInfo; ptr; ptr = ptr->ai_next) {
        // getaddrinfo() means we don't have to hardcode values here
        sockFD = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (sockFD == -1) {
            continue;
        }


        fcntl(sockFD, F_SETFL, O_NONBLOCK);

        if (connect(sockFD, ptr->ai_addr, ptr->ai_addrlen) == -1) {
            if(errno != EINPROGRESS){
                ss << "Connecting to " << hostname
                   << " failed: "<< strerror(errno);
                close(sockFD);
                freeaddrinfo(servInfo);
                throw runtime_error(ss.str());
            }
        }


        fd_set set;
        FD_ZERO(&set);
        FD_SET(sockFD, &set);

        if(select(sockFD+1, NULL, &set, NULL, &timeout) != 1){
            close(sockFD);
            freeaddrinfo(servInfo);
            ss << "Connection to " << hostname << " timed out";
            throw runtime_error(ss.str());
        }

        fcntl(sockFD, F_SETFL, fcntl(sockFD, F_GETFL, 0) & ~O_NONBLOCK);

        break;
    }

    freeaddrinfo(servInfo);

    if (!ptr) {
        close(sockFD);
        ss << "Failed to connect to " << hostname << " on port " << port;
        throw runtime_error(ss.str());
    }

    connected = true;
}
