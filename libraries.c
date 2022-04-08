/*
Author: Chris Cohen
Major: Comp Sci 
Creation Date: 11/16/2021
Due Date: 12/02/2021
Course: CSC 328
Professor Name: Dr. Frye
Assignment: Rock, Paper, Scissors
Filename: libraries.c
Purpose: C file holding the specific functions and what they do 
Language: C Language (gcc version 8.3) 
Compile: 
Execution: 
Cite: 
*/


#include "libraries.h"

/*
Name: sendMsg
Description: Function used to send a message for either the client or server, depending on who is currently using the function. 
Parameters:				int sock: Address of the server. Is put into write() as the first parameter (meaning the header and message is written into it)  
						char* msg: Holds the current message that is being sent. Used to get the character size of the message and add the message to the header
Return value: None (the write function puts the message into sock)
*/
void sendMsg(int sock, char* msg) {
    int msgSize = sizeof(msg); 
    char* header = (msgSize%100)+""; // converts the size to a char* of max length 2
    char base[] = "0";               //used if the length of the message is <10 ie only 1 digit

    // adds a "0" to the beggining of the header if it is too small
    if (msgSize < 10) {
        strcat(base, header);
        header = base;
    }
    if (msgSize == 0)              
        header = "99";   // if nickname is a multiple of 100
    strcat(header, msg); // adds the header to the beggining and sends to the socket
	//does the write system call and an error check foerrno
   if (write(sock, header, msgSize) == -1){
	   perror("Error reading pipes: ");
	   exit(-1);
   }
	return 0;
}

/*
Name: getMsg
Description: Function used to get the message from the client or server, depending on who just used "sendMsg"
Parameters:				int sock: Address of the server (likely a pipe side). Holds the header (size of the message) and message which will be the first parameter of read() to read into "msg"
Return value: None (the read function puts the message into sock)
*/
char* getMsg(int sock) {
    char* msg = malloc(sizeof(char) * 100);
    int msgSize = 0;
    msg = "";

    // first 2 chars are size of message, max of 99 chars for nickname
    if (read(sock, msg, 2) == -1){
		perror("Error writing pipes: ");
		exit(-1);
	}
    msgSize = atoi(msg);
	//does the read system call and an error check for errno 
    if (read(sock, msg, msgSize) == -1){
		perror("Error writing pipes: ");
		exit(-1);
	}
    return 0;
}

/*
Name: arrayName
Description: Prints out a statement to indicate which player made which move. 
Parameters:				int i: Holds the value to indicate which player it is
						int v: Holds the value to indicate which move was played between the array below
Return value: None (simply prints out a statement before returning)
*/

void arrayName (int i, int v){
	char array[][25] = {"ROCK", "PAPER", "SCISSORS"};
	printf("Player %d returned %s ", i, array[v]);
	return 0;
}

/*
Name: tictactoe
Description: Function deciding who won the match (or if it were a tie) and then incrementing the current score values. 
Parameters:				int i: Holds the value to indicate which move was made by player 1
						int v: Holds the value to indicate which move was made by player 2
						int *score1: Holds the current pointer score value of player 1
						int *score2: Holds the current pointer score value of player 2
Return value: None (the pointers have the scores return without anything else needing to be returned as it is a void function)
*/

void tictactoe (int i, int v, int *score1, int *score2) {
	
	int hold;
	if (i == v){
		printf("The game is a tie.");
	}
	if (i > v){
		hold = i - v;
		if (hold = 1) {
			printf("Player 1 wins");
			(*score1)++; 
		}
		else if (hold = 2){
			printf("Player 2 wins");
			(*score2)++;
		}
	}
	
	if (i < v){
		hold = v - i;
		if (hold = 1) {
			printf("Player 2 wins");
			(*score2)++;
		}
		else if (hold = 2) {
			printf("Player 1 wins");
			(*score1)++;
		}
	}
	printf("\n%d %d\n", *score1, *score2);
	return;
}