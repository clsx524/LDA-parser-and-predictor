#ifndef _TRANSMIT_H
#define _TRANSMIT_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include "model.h"
#include "strtokenizer.h"
#include "constants.h"
#include "socket.h"

using namespace std;

class transmit : private socket {

public:    
    transmit(const int port) : socket(port) {
        if (!socket::socketEstablish()) 
            throw SocketException("Could not create server socket");
        if (!socket::socketBind()) 
            throw SocketException("Could not bind to port");
        if (!socket::socketListen())  
            throw SocketException("Could not listen to socket");          
    }
    transmit() {}
    ~transmit() {}
    
    const transmit& operator << (const string& str) const;
    const transmit& operator >> (string& str) const;
    
    void accept(transmit& clt);

    void SendFile(const vector<string>& info);

    void SendStruct(string arg);

    void SetContent(struct MovieData &movie, vector<string>& arg);

    void setPort(int p) {
        socket::setPort(p);
    }

    void close();
    
};

#endif

