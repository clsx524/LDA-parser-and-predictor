#ifndef _SOCKET_H
#define _SOCKET_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h> 
#include <string>
#include <arpa/inet.h>
#include "constants.h"

using namespace std;

class socket {
private:
    int sockfd;
    struct sockaddr_in addr;
    string host;
public:
    socket(const int p, const string& h) : sockfd(-1), host(h) {
        bzero(&addr,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(p);
    }

    socket(const string& h) : sockfd(-1), host(h) {
        bzero(&addr,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(PORT);
    }

    socket(const int p) : sockfd(-1) {
        bzero(&addr,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(p);
    }  

    socket() : sockfd(-1) {
        bzero(&addr,sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(PORT);
    } 

    socket(socket& sock) {
        *this = sock;
    } 

    ~socket() {
        if (is_valid())
            close(sockfd);
    }
    
    // Server initialization
    bool socketEstablish();
    bool socketBind();
    bool socketListen() const;
    bool socketAccept(socket& sock) const;
    
    // // Client initialization
    // bool connect();
    
    // Data Transimission
    bool send (const string& str) const;
    bool send(char *block, int length) const;
    int recv (string& str) const;
    
    
    //void set_non_blocking (const bool b);
    
    bool is_valid() const { return sockfd != -1; }

    void setPort(int p) {
        addr.sin_port = htons(p);
    }

    // string getLocalAddress();
};


#endif