CC=g++
CCFLAGS = -pedantic -Wall -Werror -g -O3

MYSQL =  -L/usr/local/zend/mysql/lib  -L/Users/eric/Dropbox/sources/mysql-connector-c++-1.1.2/cppconn -L/Users/eric/Dropbox/sources/mysql-connector-c++-1.1.2/driver /Users/eric/Dropbox/sources/mysql-connector-c++-1.1.2/driver/libmysqlcppconn-static.a -lmysqlclient_r -lpthread

BOOST = /usr/local/lib/libboost_regex-mt.a

all: lda data

data: database.o utils.o strtokenizer.o data.cpp 
	$(CC) $(CCFLAGS) -o data data.cpp database.o utils.o strtokenizer.o $(MYSQL) $(BOOST) -lm 
	strip data

lda: dataset.o strtokenizer.o utils.o model.o database.o lda.cpp
	$(CC) $(CCFLAGS) -o lda lda.cpp dataset.o strtokenizer.o utils.o model.o database.o $(MYSQL) $(BOOST) -lm
	strip lda

strtokenizer.o:	strtokenizer.h strtokenizer.cpp
	$(CC) $(CCFLAGS) -c strtokenizer.cpp

dataset.o:	dataset.h dataset.cpp
	$(CC) $(CCFLAGS) -c dataset.cpp

utils.o:	utils.h utils.cpp
	$(CC) $(CCFLAGS) -c utils.cpp

model.o:	model.h model.cpp
	$(CC) $(CCFLAGS) -c model.cpp

database.o: database.h database.cpp
	$(CC) $(CCFLAGS) -c database.cpp

test:
	

clean:
	rm -rf *.o *~ lda data

