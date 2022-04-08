/*
Author: Chris Cohen
Major: Comp Sci 
Creation Date: 11/16/2021
Due Date: 12/02/2021
Course: CSC 328
Professor Name: Dr. Frye
Assignment: Rock, Paper, Scissors
Filename: libraries.h
Purpose: Header file for the library functions that both the client and server use
Language: C Language (gcc version 8.3) 
Compile: 
Execution: 
Cite: 
*/

#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
Name: sendMsg
Description: Function used to send a message for either the client or server, depending on who is currently using the function. 
Parameters:				int sock: Address of the server. Is put into write() as the first parameter (meaning the header and message is written into it)  
						char* msg: Holds the current message that is being sent. Used to get the character size of the message and add the message to the header
Return value: None (the write function puts the message into sock)
*/

void sendMsg(int sock, char* msg);

/*
Name: getMsg
Description: Function used to get the message from the client or server, depending on who just used "sendMsg"
Parameters:				int sock: Address of the server. Holds the header (size of the message) and message which will be the first parameter of read() to read into "msg"
Return value: None (the read function puts the message into sock)
*/

char* getMsg(int sock);


/*
Name: arrayName
Description: Prints out a statement to indicate which player made which move. 
Parameters:				int i: Holds the value to indicate which player it is
						int v: Holds the value to indicate which move was played between the array below
Return value: None (simply prints out a statement before returning)
*/

void arrayName (int i, int v);

/*
Name: tictactoe
Description: Function deciding who won the match (or if it were a tie) and then incrementing the current score values. 
Parameters:				int i: Holds the value to indicate which move was made by player 1
						int v: Holds the value to indicate which move was made by player 2
						int *score1: Holds the current pointer score value of player 1
						int *score2: Holds the current pointer score value of player 2
Return value: None (the pointers have the scores return without anything else needing to be returned as it is a void function)
*/

void tictactoe (int i, int v, int *score1, int *score2);