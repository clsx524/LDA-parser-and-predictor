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
#include <stringstream>
#include "model.h"
#include "strtokenizer.h"
#include "constants.h"

using namespace std;

class client : private socket {
    string url;
    
public:
    client(const int p, const string& u) : socket(p, u) {}
    client(socket& sock) :socket(sock) {}
    ~client() {}

    int SockEstablish();
    
    void ClientConnect(int clientfd, char* addr);
    
    void SaveContent(int clientfd, char* addr);
    
    void SaveStruct(int clientfd);

    void SendCommand(int clientfd, string command);
    
    void GetResponse(int clientfd);

    string getForeignAddress();
};