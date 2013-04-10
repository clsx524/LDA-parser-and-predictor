#include "client.h"

int client::SockEstablish(){
	struct sockaddr_in clientaddr;
	bzero(&clientaddr, sizeof(clientaddr));

	clientaddr.sin_family = AF_INET;
	clientaddr.sin_addr.s_addr = htons(INADDR_ANY);
	clientaddr.sin_port = htons(0);

	int clientfd = socket(AF_INET, SOCK_STREAM,0);

	if (clientfd < 0) {
		perror("socket");
		exit(1);
	}

	if (bind(clientfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0) {
		perror("bind");
		exit(1);
	}
	return clientfd;
}

void client::ClientConnect(int clientfd, char* addr) {
	struct sockaddr_in svraddr;
	bzero(&svraddr, sizeof(svraddr));
	if (inet_aton(addr, &svraddr.sin_addr) == 0) {
		perror("inet_aton");
		exit(1);
	}
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(PORT);

	socklen_t svraddrlen = sizeof(svraddr);
	if (connect(clientfd, (struct sockaddr*)&svraddr, svraddrlen) < 0) {
		perror("connect");
		exit(1);
	}
}

void client::SaveContent(int clientfd, char* addr) {
        //recv file imformation
	char buff[BUFFSIZE];
	bzero(buff,BUFFSIZE);
	char filename[FILE_NAME_MAX_SIZE];
	int count;

	count = recv(clientfd, filename, FILE_NAME_MAX_SIZE, 0);
	if (count < 0) {
		perror("recv");
		exit(1);
	}

	printf("Preparing recv file : %s from %s \n",filename,addr);

        //recv file
	FILE *fd = fopen(filename, "wb+");
	if (NULL == fd) {
		perror("open");
		exit(1);
	}

	bzero(buff,BUFFSIZE);

	int length = 0;
	while (length = recv(clientfd, buff, BUFFSIZE, 0)) {
		if (length < 0) {
			perror("recv");
			exit(1);
		}
		int writelen = fwrite(buff,sizeof(char),length,fd);
		if(writelen<length){
			perror("write");
			exit(1);
		}
		bzero(buff,BUFFSIZE);
	}
	printf("Receieved file:%s from %s finished!\n",filename,addr);
	fclose(fd);
}

void client::SaveStruct(int clientfd){
	char buffer[BUFFSIZE];bzero(buffer,BUFFSIZE);
	struct MovieData recvMovie;

	int count;
	count=recv(clientfd, buffer, sizeof(buffer), 0);
	memcpy(&recvMovie, buffer, sizeof(buffer));

	cout << "Movie name is " << recvMovie.name << endl;
	cout << "Movie year is " << recvMovie.year << endl;
	cout << "Movie length is " << recvMovie.length << endl;
	cout << "Movie director is " << recvMovie.director << endl;
	cout << "Movie cast is " <<recvMovie.cast << endl;
	cout << "Movie content is " << recvMovie.content << endl;
	cout << "Movie wiki is " << recvMovie.wiki << endl;
}

void client::SendCommand(int clientfd, string command){
	int count2 = send(clientfd,command.c_str(),WORD_MAX_SIZE,0);
	if(count2 < 0){
		perror("connect");
		exit(1);
	}
}

void client::GetResponse(int clientfd){
	char response[WORD_MAX_SIZE]; bzero(response,WORD_MAX_SIZE);
	int count4=recv(clientfd,response,WORD_MAX_SIZE,0);
	if(count4<0){
		perror("recv");
		exit(1);
	}
	cout << response << endl; bzero(response,WORD_MAX_SIZE);
}

string client::getForeignAddress() {
  return inet_ntoa(socket::addr.sin_addr);
}