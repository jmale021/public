PROGS =	client server 
CC = gcc
LIBS = 


all:	${PROGS}


client:	client.o libraries.o
	gcc -o $@ client.o libraries.o ${LIBS}

server:	server.o libraries.o
	gcc -o $@ server.o libraries.o ${LIBS}

client.o: client.c
	gcc -c client.c

server.o: server.c
	gcc -c server.c
	
libraries.o: libraries.c	
	gcc -c libraries.c

clean:
	/bin/rm -f client server 
	/bin/rm -f *.o
	/bin/rm -f core
