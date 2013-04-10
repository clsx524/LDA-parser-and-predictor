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
            cout << "error1" << endl;
        if (!socket::socketBind()) 
            cout << "error2" << endl;
        if (!socket::socketListen())  
            cout << "error3" << endl;           
    }
    transmit() {}
    ~transmit() {}
    
    const transmit& operator << (const string& str) const;
    const transmit& operator >> (string& str) const;
    
    bool accept(transmit& clt);

    void SendFile(string filename, string number);

    void SendStruct(vector<string>& arg);

    void SetContent(struct MovieData &movie, vector<string>& arg);

    void setPort(int p) {
        socket::setPort(p);
    }
    
};

#endif

