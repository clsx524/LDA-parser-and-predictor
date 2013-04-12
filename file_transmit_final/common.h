#include <iostream> 
#include <string>
#include <stdio.h> 
#include <stdlib.h>  
#include <unistd.h> 
#include <string.h>   
#include <sys/types.h>  
#include <sys/stat.h>  
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <fstream> 
#include <vector>
#include <assert.h>
      
#define PORT 6000  
#define LISTENQ 20  
#define BUFFSIZE 4096  
#define FILE_NAME_MAX_SIZE 512
#define WORD_MAX_SIZE 256 

struct MovieData {
    char name[100];
    char year[10];
    char length[10];
    char director[30];
    char cast[500];
    char pic[200];
    char number[20];
    char content[10000];
    char wiki[10000];
};

using namespace std;


/************************************************************************/
/************************************************************************/
/*******************Function codes for client side***********************/
/************************************************************************/
/************************************************************************/

vector<string> split(string str, string seperators) {
    int n = str.length();
    int start, stop;
    vector<string> tokens;
    
    start = str.find_first_not_of(seperators);
    while (start >= 0 && start < n) {
        stop = str.find_first_of(seperators, start);
        if (stop < 0 || stop > n) {
            stop = n;
        }
        tokens.push_back(str.substr(start, stop - start));
        start = str.find_first_not_of(seperators, stop + 1);
    }
    return tokens;
}

int Clie_SockEstablish(){
    struct sockaddr_in clientaddr;  
    bzero(&clientaddr,sizeof(clientaddr));    
          
    clientaddr.sin_family=AF_INET;  
    clientaddr.sin_addr.s_addr=htons(INADDR_ANY);  
    clientaddr.sin_port=htons(0);

    int clientfd=socket(AF_INET,SOCK_STREAM,0);  
              
    if(clientfd<0){  
        perror("socket");  
        exit(1); 
    }  

    if(bind(clientfd,(struct sockaddr*)&clientaddr,sizeof(clientaddr))<0){  
        perror("bind");  
        exit(1);
    }  
    return clientfd;
}

void Clie_ClientConnect(int clientfd, char* addr){
    struct sockaddr_in svraddr;  
    bzero(&svraddr,sizeof(svraddr));  
    if(inet_aton(addr,&svraddr.sin_addr)==0){  
        perror("inet_aton");  
        exit(1);
    }  
    svraddr.sin_family=AF_INET;  
    svraddr.sin_port=htons(PORT);  
              
    socklen_t svraddrlen=sizeof(svraddr);  
    if(connect(clientfd,(struct sockaddr*)&svraddr,svraddrlen)<0){  
        perror("connect");  
        exit(1); 
    } 
}

void Clie_SaveContent(int clientfd, char* addr, string filename){
    //recv file imformation  
    char buff[BUFFSIZE];bzero(buff,BUFFSIZE);  
    //char filename[FILE_NAME_MAX_SIZE] = "a.jpg";    
              
    cout << *addr << filename << endl;   
              
    //recv file  
    FILE *fd=fopen(filename.c_str(),"wb+");  
    if(NULL==fd){  
        perror("open");  
        exit(1);   
    }  
            
    bzero(buff,BUFFSIZE);  
              
    int length=0;  
    while((length=recv(clientfd,buff,BUFFSIZE,0))){  
        if(length<0){  
            perror("recv");  
            exit(1);  
        }  
        int writelen=fwrite(buff,sizeof(char),length,fd);  
        if(writelen<length){  
            perror("write");  
            exit(1);   
        }  
        bzero(buff,BUFFSIZE);  
    }  
    printf("Receieved file:%s from %s finished!\n",filename.c_str(),addr);  
    fclose(fd);
}

string Clie_SaveStruct(int clientfd){
    char buffer[BUFFSIZE];bzero(buffer,BUFFSIZE);
    struct MovieData recvMovie;

    //int count;
    recv(clientfd, buffer, sizeof(buffer), 0);
    memcpy(&recvMovie, buffer, sizeof(buffer));

    cout << "Title is " << recvMovie.name << endl;
    cout << "Year is " << recvMovie.year << endl;
    cout << "Length is " << recvMovie.length << endl;
    cout << "Director is " << recvMovie.director << endl;
    cout << "Cast is " <<recvMovie.cast << endl;
    cout << "Content is " << recvMovie.content << endl;
    cout << "Wiki is " << recvMovie.wiki << endl;
    cout << "Pic is " << recvMovie.pic << endl;
    cout << "Number is " << recvMovie.number << endl;
    return recvMovie.pic;
}

void Clie_SendCommand(int clientfd, string command){
    int count2 = send(clientfd,command.c_str(),WORD_MAX_SIZE,0);
    if(count2 < 0){
        perror("connect");
        exit(1);                  
    }    
}
      
string Clie_GetResponse(int clientfd){
    char response[WORD_MAX_SIZE];bzero(response,WORD_MAX_SIZE);
    int count4=recv(clientfd,response,WORD_MAX_SIZE,0);  
    if(count4<0){  
        perror("recv");  
        exit(1);  
    }
    cout << response << endl;
    string re(response);
    cout << re << "1" << endl;
    bzero(response,WORD_MAX_SIZE);
    return re;
}





/************************************************************************/
/************************************************************************/
/*******************Function codes for server side***********************/
/************************************************************************/
/************************************************************************/
int Serv_SockEstablish(){
    struct sockaddr_in svraddr;  
    bzero(&svraddr,sizeof(svraddr));  
              
    svraddr.sin_family=AF_INET;  
    svraddr.sin_addr.s_addr=htonl(INADDR_ANY);  
    svraddr.sin_port=htons(PORT); 

    int sockfd=socket(AF_INET,SOCK_STREAM,0);  
    if(sockfd<0){  
        perror("socket");  
        return 1;  
    }  
          
    //bind    
    if(bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr))<0){  
        perror("bind");  
        return 1;  
    }  

    //listen  
    listen(sockfd,LISTENQ);
    cout<<"Listening..."<<endl;

    return sockfd;    
}

int Serv_ConnEstablish(int sockfd){
    struct sockaddr_in clientaddr;
    socklen_t length=sizeof(clientaddr);
    //accept  
    int connfd=accept(sockfd,(struct sockaddr*)&clientaddr,&length);  
    if(connfd<0){  
        perror("connect");  
        exit(1);  
    }
    return connfd;    
}

void Serv_GetCommand(int connfd, char (&command)[WORD_MAX_SIZE]){
    if(recv(connfd,command,WORD_MAX_SIZE,0)==-1){  
        perror("recv");  
        exit(1);   
    }
}

void Serv_SendFile(int connfd_temp, string filename){
    char buff[BUFFSIZE];bzero(buff,BUFFSIZE);  
    int count;              
    count=send(connfd_temp, filename.c_str() , FILE_NAME_MAX_SIZE ,0);  
    if(count<0){  
        perror("Send file imformation");  
        exit(1);  
    }  
                      
    //read file   
    FILE *fd=fopen(filename.c_str(),"rb");  
    if(fd==NULL){  
        cout << "File : " << filename << " not found!" << endl;  
    }else{  
        bzero(buff,BUFFSIZE);  
        int file_block_length=0;  
        while((file_block_length=fread(buff,sizeof(char),BUFFSIZE,fd))>0){  
            cout << "file_block_length:" << file_block_length << endl;  
            if(send(connfd_temp,buff,file_block_length,0)<0){  
                perror("Send");  
                exit(1);  
            }  
            bzero(buff,BUFFSIZE);     
        }  
        fclose(fd);  
        printf("Transfer file finished !\n");  
    }
}

void Serv_SendStruct(int connfd_temp, struct MovieData &movie){               
    if(send(connfd_temp,(char*)&movie,sizeof(movie),0)<0){  
        perror("Send");  
        exit(1);  
    }  
    printf("Struct file finished !\n");     
}


void Serv_SetContent(struct MovieData &movie, string name, string year, string length, string director, string cast, string content, string wiki){
    memcpy(movie.name, name.c_str(), name.size());
    memcpy(movie.year, year.c_str(), year.size());
    memcpy(movie.length, length.c_str(), length.size());
    memcpy(movie.director, director.c_str(), director.size());
    memcpy(movie.cast, cast.c_str(), cast.size());
    memcpy(movie.content, content.c_str(), content.size());
    memcpy(movie.wiki,wiki.c_str(), wiki.size());
}