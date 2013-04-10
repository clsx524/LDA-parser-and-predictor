#include "transmit.h"

bool transmit::accept(transmit& clt) {
    if (!socket::socketAccept(clt)) {
        cout << "error" << endl;
        return false;
    }
    cout << "connected" << endl;
    return true;
}

const transmit& transmit::operator<<(const string &str) const {   
    if (!socket::send(str))
        cout << "error" << endl;
    return *this;
}


const transmit & transmit::operator>>(string &str) const {
    if (!socket::recv(str))
        cout << "error" << endl;
    return *this;
}

void transmit::SendFile(string filename, string number) {
    char buff[BUFFSIZE];bzero(buff,BUFFSIZE);
    if (!socket::send(number))
        cout << "error" << endl;
    
    //read file
    FILE *fd = fopen(filename.c_str(),"rb");
    if (fd == NULL) {
        cout << "File : " << filename << " not found!" << endl;
    } else {
        bzero(buff,BUFFSIZE);
        int file_block_length = 0;
        while ((file_block_length = fread(buff,sizeof(char),BUFFSIZE,fd)) > 0) {
            cout << "file_block_length:" << file_block_length << endl;
            if(!socket::send(buff,file_block_length)){
                perror("Send");
                exit(1);
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
        perror("Send");
        exit(1);
    }
    printf("Struct file finished !\n");
}

void transmit::SetContent(struct MovieData &movie, vector<string>& arg) {
    memcpy(movie.name, arg[0].c_str(), arg[0].size());
    memcpy(movie.year, arg[1].c_str(), arg[1].size());
    memcpy(movie.length, arg[2].c_str(), arg[2].size());
    memcpy(movie.director, arg[3].c_str(), arg[3].size());
    memcpy(movie.cast, arg[4].c_str(), arg[4].size());
    memcpy(movie.content, arg[5].c_str(), arg[5].size());
    memcpy(movie.wiki,arg[6].c_str(), arg[6].size());
}


