#include "socket.h"

bool socket::socketEstablish(){
    sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!is_valid())
        return false;
    
    int on = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(on)) == -1)
        return false;
    
    return true;
}

bool socket::socketBind() {
    if (!is_valid())
        return false;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        return false;
    return true;
}

bool socket::socketListen() const {
    if (!is_valid())
        return false;
    if(listen(sockfd, LISTENQ) < 0)
        return false;
    cout<<"Listening..."<<endl;
    return true;
}

bool socket::socketAccept(socket& sock) const {
    cout << "here2" << endl;
    socklen_t length = sizeof(sock.addr);
    sock.sockfd = ::accept(sockfd, (sockaddr*) &sock.addr, &length);
    cout << sock.sockfd << endl;
    if (sock.sockfd <= 0)
        return false;
    return true;
}

bool socket::send(const string& str) const {
    int status = ::send(sockfd, str.c_str(), WORD_MAX_SIZE, 0);
    if (status == -1)
        return false;
    return true;
}

bool socket::send(char *block, int length) const {
    int status = ::send(sockfd, block, length, 0);
    if (status == -1)
        return false;
    return true;
}

int socket::recv (string& str) const {
    char buff[BUFFSIZE]; bzero(buff,BUFFSIZE);
    int status = ::recv(sockfd, buff, WORD_MAX_SIZE, 0);
    
    if (status == -1) {
        cout << "error1" << endl;
        return 0;
    } else if (status == 0)
        return 0;
    else {
        str = buff;
        return status;
    }
}

void socket::close() {
    ::close(sockfd);
}

bool socket::connect () {
    if (!is_valid()) return false;
    
    int status = inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    
    if (errno == EAFNOSUPPORT) return false;
    
    status = ::connect(sockfd, (sockaddr*) &addr, sizeof(addr));
    
    if (status == 0)
        return true;
    return false;
}

// void socket::set_non_blocking (const bool b) {
//     int opts;
//     opts = fcntl(sockfd, F_GETFL);
    
//     if (opts < 0)
//         return;
    
//     if (b)
//         opts = (opts | O_NONBLOCK);
//     else
//         opts = (opts & ~O_NONBLOCK);
    
//     fcntl(sockfd, F_SETFL, opts);
// }

SocketException::SocketException(const string &message, bool inclSysMsg)
  throw() : userMessage(message) {
  if (inclSysMsg) {
    userMessage.append(": ");
    userMessage.append(strerror(errno));
  }
}

const char *SocketException::what() const throw () {
  return userMessage.c_str();
}

