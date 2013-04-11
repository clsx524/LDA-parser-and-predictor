# Makefile for the socket programming example
#

all : fileserver fileclient

fileserver: fileserver.cpp common.h
	g++ -pedantic -Wall -Werror -g -O3 -o fileserver -g fileserver.cpp

fileclient: fileclient.cpp common.h
	g++ -pedantic -Wall -Werror -g -O3 -o fileclient -g fileclient.cpp
	
clean:
	rm -f *.o fileserver fileclient
