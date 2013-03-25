CC=g++
CCFLAGS = -pedantic -Wall -Werror -g -O3

all: main

main: dataset.o strtokenizer.o utils.o model.o lda.cpp
	$(CC) $(CCFLAGS) -o lda lda.cpp dataset.o strtokenizer.o utils.o model.o
	strip lda

strtokenizer.o:	strtokenizer.h strtokenizer.cpp
	$(CC) $(CCFLAGS) -c strtokenizer.cpp

dataset.o:	dataset.h dataset.cpp
	$(CC) $(CCFLAGS) -c dataset.cpp

utils.o:	utils.h utils.cpp
	$(CC) $(CCFLAGS) -c utils.cpp

model.o:	model.h model.cpp
	$(CC) $(CCFLAGS) -c model.cpp

test:
	

clean:
	rm -rf *.o *~ lda

