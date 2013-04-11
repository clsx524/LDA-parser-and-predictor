#include "transmit.h"

void transmit::accept(transmit& clt) {
    if (!socket::socketAccept(clt)) {
        throw SocketException("Could not accept socket");
    }
    cout << "connected" << endl;
}

void transmit::close() {
    socket::close();
}

const transmit& transmit::operator << (const string &str) const {   
    if (!socket::send(str))
        throw SocketException("Could not write to socket");
    return *this;
}


const transmit & transmit::operator >> (string &str) const {
    if (!socket::recv(str))
        throw SocketException("Could not read from socket");
    return *this;
}

void transmit::SendFile(const vector<string>& info) {
    assert(info.size() == 3);
    string part = info[0] + "||||" + info[1] + "||||" + info[2];
    cout << part << endl;
    if (!socket::send(part))
        throw SocketException("Could not send to socket");

    char buff[BUFFSIZE];bzero(buff,BUFFSIZE);    
    //read file
    FILE *fd = fopen(info[2].c_str(),"rb");
    if (fd == NULL) {
        throw SocketException("Could not find a file");
    } else {
        cout << "file found" << endl;
        bzero(buff,BUFFSIZE);
        int file_block_length = 0;
        while ((file_block_length = fread(buff,sizeof(char),BUFFSIZE,fd)) > 0) {
            cout << "file_block_length:" << file_block_length << endl;
            if(!socket::send(buff,file_block_length)) {
                throw SocketException("Could not send file");
            }
            bzero(buff,BUFFSIZE);
        }
        fclose(fd);
        printf("Transfer file finished !\n");
    }
}

void transmit::SendStruct(vector<string>& arg){
    struct MovieData movie;
    SetContent(movie, arg);
    if(!socket::send((char*)&movie,sizeof(movie))) {
        throw SocketException("Could not send struct");
    }
    printf("Struct file finished !\n");
}

void transmit::SetContent(struct MovieData &movie, vector<string>& arg) {
    memcpy(movie.name, arg[0].c_str(), arg[0].size()); cout << arg[0] << endl;
    memcpy(movie.year, arg[1].c_str(), arg[1].size()); cout << arg[1] << endl;
    memcpy(movie.length, arg[2].c_str(), arg[2].size()); cout << arg[2] << endl;
    memcpy(movie.director, arg[3].c_str(), arg[3].size()); cout << arg[3] << endl;
    memcpy(movie.cast, arg[4].c_str(), arg[4].size()); cout << arg[4] << endl;
    memcpy(movie.content, arg[5].c_str(), arg[5].size()); cout << arg[5] << endl;
    memcpy(movie.wiki,arg[6].c_str(), arg[6].size()); cout << arg[6] << endl;
    memcpy(movie.pic,arg[7].c_str(), arg[7].size()); cout << arg[7] << endl;
    memcpy(movie.number,arg[8].c_str(), arg[8].size()); cout << arg[8] << endl;
}


