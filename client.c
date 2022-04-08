//  Author: Joshua Malek 
//  Major: Computer Science 
//  Creation Date: 11/16/2021
//  Due Date: 12/2/2021
//  Course: CSC328 
//  Professor Name: Dr. Frye 
//  Filename: client.c
//  Purpose: Client side for playing rock paper scissors.

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#define PORT 12001

#define NICK  "NICK"
#define READY "READY"
#define RETRY "RETRY"
#define GO    "GO"
#define SCORE "SCORE"
#define STOP  "STOP"
#define OK    "OKAY"

void sendMsg(int sock, char* msg);
char* getMsg(int sock);
void getReady(int sock);
void getNickname(int sock);
char* playRound(int sock); 

struct INFO { 
    struct sockaddr_in *address; 
    int fd; 
}; 

struct INFO getAddressInfo( char* hostname, int Port){ 

    int server_fd, rv; 
    int type; 
    struct sockaddr_in *address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    struct addrinfo *server_hints = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    struct addrinfo *server_info = (struct addrinfo *)malloc(sizeof(struct addrinfo));
    struct INFO returns; 
    // These conditionals figure out what protocl was specified and saves it as an int. This allows me 
    // to create a socket for either UDP or TCP with one fucntion 
    server_hints->ai_family = AF_INET;  // AF_INET = IPV4
    server_hints->ai_flags = 0; 
    server_hints->ai_protocol = 0; 
    server_hints->ai_socktype = SOCK_STREAM; // type is either SOCK_STREAM or SOCK_DGRAM

    rv = getaddrinfo(hostname, NULL, server_hints, &server_info);  // getting address info about the hostname 
    if (rv < 0) {
	printf("Error in getaddrinfo call: %s\n", gai_strerror(rv));
	exit(-1);
    } 

    address = (struct sockaddr_in *)server_info->ai_addr; 
    address->sin_port = htons(Port); 

    server_fd = socket(AF_INET, SOCK_STREAM, 0); // creates socket 
    if (server_fd == -1) {
        perror("Error creating client socket");
        exit(-1);
        }   // end if error socket
    
    returns.fd = server_fd; 
    returns.address = address; 

    return returns;
}

int main(int argc, char **argv){ 

    // Variables 
    int server_fd, rv; 
    struct sockaddr_in *address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in)); 
    char* hostname = argv[1]; 
    struct INFO info; 
    int Port; 
  
    if (argc > 3){ 
        printf("Usage: %s [hostname] [Port] \n", argv[0]); 
        exit(0); 
    }

    if (argc == 3){ 
        Port = atoi(argv[2]); 
    } else{ 
        Port = PORT; 
    }

   info = getAddressInfo(argv[1], Port); // creates socket and returns addressing info about the hostname 
   address = info.address; 
   server_fd = info.fd; 
   rv = connect(server_fd, (struct sockaddr *) address, sizeof(struct sockaddr)); // connect to server
   if (rv == -1)  {  // error
    perror("Error connecting to server");
    exit(-1);
    }  // end if

    getReady(server_fd); 
    getNickname(server_fd); 
    playRound(server_fd); 

    return 0; 
}

void sendMsg(int sock, char* msg) {
    int msgLen;
    char* header = malloc(sizeof(char) * 1000); // converts the size to a char* of max length 2
    char base[100] = "0";               //used if the length of the message is <10 ie
    msgLen = strlen(msg);
    sprintf(header, "%d", msgLen);

    // adds a "0" to the beggining of the header if it is too small
    if (msgLen < 10) {
        strcat(base, header);
        header = base;
    }
    
    strcat(header, msg); // adds the header to the beggining and sends to the socket
    
    write(sock, header, strlen(header));
    
    return;
}

char* getMsg(int sock) {
    char* msg = malloc(sizeof(char) * 100);
	char* head = malloc(sizeof(char) * 100);
    int msgSize = 0;
    // first 2 chars are size of message, max of 99 chars for nickname
    read(sock, head, 2);
    msgSize = atoi(head);
    read(sock, msg, msgSize);

    return msg;
}

void getReady(int sock) {
    printf("Readying\n");
    sendMsg(sock, "READY"); 
}

void getNickname(int sock) {
    char* nick = malloc(sizeof(char) * 100);
    char* response = malloc(sizeof(char) * 100);
    do {
        printf("Please enter a unique nickname:\n");
        scanf("%s", nick);
        //write(sock, nick, strlen(nick)); 
        sendMsg(sock, nick);
        //read(sock, response, 100);
        response = getMsg(sock);
    } while (strcmp(response, "READY") != 0);
    return;
}

char* playRound(int sock) {
    char* move = malloc(sizeof(char) * 100);
    char* response = malloc(sizeof(char) * 100);
    char* score = malloc(sizeof(char) * 100);

    do{ 
        response = getMsg(sock); 
    }while(strcmp(response, "READY") != 0); 

    sendMsg(sock, "GO"); 

    while(1){ 
        response = getMsg(sock);
        if (strcmp(response, "STOP") ==0){ 
            printf("Response: %s\n", response); 
            break; 
        }
        printf("Please enter a move- R, P, or S: <Choice>\n");
        scanf("%s", move);
        sendMsg(sock, move);
        printf("Response: %s\n", response); 
    } 
   
    score = getMsg(sock);
    printf("%s\n", score); 
}

