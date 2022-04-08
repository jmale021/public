#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>



#define LISTENQ   1024
#define PORT 12001

#define NICK  "NICK"
#define READY "READY"
#define RETRY "RETRY"
#define GO    "GO"
#define SCORE "SCORE"
#define STOP  "STOP"
#define OK    "OKAY"


#define READ 0 
#define WRITE 1

int sprintf ( char * str, const char * format, ... );
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

void READYCLIENT(int connection, int child_read, int child_write){  
	char* client_response = malloc(sizeof(char) * 100); 
	char* parent_response = malloc(sizeof(char) * 100); 

	client_response = getMsg(connection); 
 
	sendMsg(child_write, client_response); 
	parent_response = getMsg(child_read); 
 
	if (strcmp(parent_response, "OK") == 0){ 
		printf("Client Ready\n"); 
		return; 
	}
	printf("ERROR in Child Code: READYCLIENT\n"); 
}

void READYCLIENTS(int child_1_write, int child_1_read, int child_2_write, int child_2_read){ 
	printf("Readying clients\n"); 
	char* child_1_response = malloc(sizeof(char) * 100);
	char* child_2_response = malloc(sizeof(char) * 100);

	child_1_response = getMsg(child_1_read); 
	child_2_response = getMsg(child_2_read); 
	printf("Response from child1: %s\n", child_1_response);
	printf("Response from child2: %s\n", child_2_response); 
	if ( (strcmp(child_1_response, READY) == 0) && (strcmp(child_2_response, READY) == 0)){ 
		printf("Refree says clients are ok\n"); 
		sendMsg(child_1_write, "OK"); 
		sendMsg(child_2_write, "OK"); 
	} 
	return;
}

void GETNICK(int connection, int child_read, int child_write) {

	char* client_response = malloc(sizeof(char) * 100);
	char* parent_response = malloc(sizeof(char) * 100);

	while (1) {
		printf("child reading nickname\n");
		//read(connection, client_response, 100); 
		client_response = getMsg(connection);

		//write(child_write, client_response, strlen(client_response)); 
		sendMsg(child_write, client_response);
		parent_response = getMsg(child_read);
		if (strcmp(parent_response, "OK") == 0) {

			sendMsg(connection, "READY");
			sendMsg(child_write, "READY");
			return;
		}
		else {
			sendMsg(connection, "RETRY");
			continue;
		}
	} // end of while loop 
}

void GETNICKS(int child_1_write, int child_1_read, int child_2_write, int child_2_read){ 
	printf("GET NICKS\n"); 

	char* child_1_response = malloc(sizeof(char) * 100);
	char* child_2_response = malloc(sizeof(char) * 100);
	
	while (1){ 
		child_1_response = getMsg(child_1_read);
		//read(child_2_read, child_2_response, 100); 
		child_2_response = getMsg(child_2_read);

		printf("Response Nick from child1: %s\n", child_1_response);
		printf("Response Nick from child2: %s\n", child_2_response); 


		if (strcmp(child_1_response, child_2_response)== 0){
			printf("SAME NICKNAME\n"); 
			//write(child_1_write, RETRY, strlen(RETRY));
			
			sendMsg(child_1_write, "NICK");
			sendMsg(child_2_write, "NICK");			
			continue;
		} else { 
			printf("Refree says clients Nick are ok\n"); 
			sendMsg(child_1_write, "OK");
			sendMsg(child_2_write, "OK");


			child_1_response = getMsg(child_1_read);
			child_2_response = getMsg(child_2_read);
			if ( (strcmp(child_1_response, "READY") == 0) && (strcmp(child_2_response, "READY") == 0)){
			return;
		} 
	}  // end of while loop 
	} 
	
}

void PLAYROUND(int connection, int child_read, int child_write){ 
	char* client_response = malloc(sizeof(char) * 100); 
	char* parent_response = malloc(sizeof(char) * 100); 

	char* control = malloc(sizeof(char) * 4); 

	int rv; 

	control = getMsg(child_read); 
	int rounds = atoi(control); 
	sendMsg(connection, "READY"); 

	client_response = getMsg(connection); 

	for (int i =0; i < rounds; i++){ 
		sendMsg(connection, "GO"); 
		client_response = getMsg(connection);
		printf("Move from client: %s\n", client_response); 
		sendMsg(child_write, client_response); 
	} 

	sendMsg(connection, "STOP"); 
	parent_response = getMsg(child_read);
	sendMsg(connection, parent_response); 
	
	return;  
}

void PLAYROUNDS(int child_1_write, int child_1_read, int child_2_write, int child_2_read, int rounds){ 
	printf("Play Rounds\n"); 
	int scores[2] = {0 , 0}; 
	int round = rounds; 
	signed int whoWins[3][3]; 
	// table indicating who wins using the moves as indicies into the table.
	// indices: 0 (rock), 1 (paper) , 2 (scissors)
	// values: 0 (tie), -1 (player 1 won), 1 (player 2 won)
	whoWins[0][0] =  0;  
	whoWins[1][1] =  0; 
	whoWins[2][2] =  0; 
	whoWins[0][1] =  1; 
	whoWins[0][2] = -1; 
	whoWins[1][0] =  1; 
	whoWins[1][2] = -1;  
	whoWins[2][0] =  1; 
	whoWins[2][1] = -1; 

	char* child_1_response = malloc(sizeof(char) * 100);
	char* child_2_response = malloc(sizeof(char) * 100);
	char* control = malloc(sizeof(char) * 2); 
	int whoWon; 
	int move1; 
	int move2; 

	sprintf(control, "%d", round); 
	sendMsg(child_1_write, control); 
	sendMsg(child_2_write, control);

	for (int i = 0; i < rounds; i++){ 
		child_1_response = getMsg(child_1_read); 
		child_2_response = getMsg(child_2_read);

		printf("Child Move Response: %s\n", child_1_response); 
		printf("Child Move Response: %s\n", child_2_response);  
		if(!strcmp(child_1_response , "R"))
			move1 = 0;
		if(!strcmp(child_1_response , "P"))
			move1 = 1;
		if(!strcmp(child_1_response , "S"))
			move1 = 2;
		if(!strcmp(child_2_response , "R"))
			move2 = 0;
		if(!strcmp(child_2_response , "P"))
			move2 = 1;
		if(!strcmp(child_2_response , "S"))
			move2 = 2;
		
		whoWon = whoWins[move1][move2]; 

		printf("WhoWins: %d\n", whoWon); 

		switch(whoWon){ 
			case 0: 
			continue; 
			
			case -1: 
			scores[0]++; 
			continue; 
			
			case 1: 
			scores[1]++; 
			continue; 
			
			default: 
			printf("Something went wrong\n");  
		} 
	} 

	char* score = malloc(sizeof(char) * 100); 

	sprintf(score, "Player 1: %d\nPlayer 2 %d\n", scores[0], scores[1]); 
	
	sendMsg(child_1_write, score); 
	sendMsg(child_2_write, score); 
	return; 
}

 void Child(int connection, int child_read, int child_write){  // 2 pipes per child to communicate with referee, 1 socket for each client 
    printf("Client 2 connected\n");

	READYCLIENT(connection, child_read, child_write); 
	GETNICK(connection, child_read, child_write); 
	PLAYROUND(connection, child_read, child_write); 
	close(connection); 

	exit(0); 
} 

void Referee(int child_1_write, int child_1_read, int child_2_write, int child_2_read, int listen, int rounds){ 

	READYCLIENTS(child_1_write, child_1_read, child_2_write, child_2_read); 
	GETNICKS(child_1_write, child_1_read, child_2_write, child_2_read); 
	PLAYROUNDS(child_1_write, child_1_read, child_2_write, child_2_read, rounds); 

	printf("End of referre\n"); 
	 

	return; 
} 

int main(int argc, char **argv)
{
  	int listenfd, connfd;
	pid_t childpid;
	int	clilen;
	struct sockaddr_in	cliaddr, servaddr;
	int connected = 1; 

	// PIpes and pid declarations 
    int p2c1[2]; 
    int c2p1[2]; 
	int p2c2[2]; 
	int c2p2[2]; 
    pid_t cpid; 
    pid_t ppid = getpid(); 
	int Port;
	int rounds; 

	rounds = atoi(argv[1]); 

	if (argc == 3){ 
		Port = atoi(argv[2]); 
	} else{ 
		Port = PORT; 
	}

// ======== Pipe declarations ============
    if (pipe(p2c1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    } 

    if (pipe(c2p1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    } 

	if (pipe(p2c2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    } 

    if (pipe(c2p2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    } 

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
	  perror("Error creating socket: ");
	  exit(-1);
	}   // end if socket
	
	// Assign accepting IP address and port number for socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	// accept incoming connections on any interface on the server
	servaddr.sin_addr.s_addr = INADDR_ANY;  
	// accept incoming connections on specified port number, converted to network byte order
	servaddr.sin_port        = htons(Port);

	// Bind socket to accept connections from any IP address on my specified port number
	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) == -1)
	{
	  perror("Error binding socket: ");
	  exit(-1);
	}   // end if bind

	// Put socket in listening mode (passive socket) to accept client connections
	if (listen(listenfd, LISTENQ) == -1)
	{
	  perror("Error listening socket: ");
	  exit(-1);
	}   // end if listen

	clilen = sizeof(cliaddr);


    while(1){ 
    if ( (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0){
        printf("connfd: %d\n", connfd); 
        
		perror("accept error");
		exit(-1);   // better to just do a continue??
	        // end else
	    // end if accept
    } 
    

	if ( (childpid = fork()) == 0) {

		if (connected ==1){ 
			 
			Child(connfd, p2c1[READ], c2p1[WRITE]); 
		}

		else { 
			 
			Child(connfd, p2c2[READ], c2p2[WRITE]); 
		}
      } // end child fork 

	  else{ 
		  if (connected < 2){ 
			  connected++; 
			  continue; 
		  }

		  else{ 
			  Referee(p2c1[WRITE], c2p1[READ],  p2c2[WRITE], c2p2[READ], listenfd, rounds); 
			  wait(NULL); 
			  wait(NULL); 
			  close(listenfd); 
			  exit(0); 
		  }
	  }
    } 
      return 0; 

} 