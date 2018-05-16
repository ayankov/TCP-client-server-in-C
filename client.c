/*
*	Alexei Yankovsky
* CPSC 3600 HW4
* Date due: November 21, 2017
* Instructor: Feng Luo
*
* This is the concurrent TCP client program. It connects to the TCP server at the specified port. 
* The following must be passed in on the command line:
*			IP address of the server (argv[1])
*			Port number (argv[2])
*			File name to read from (argv[3])		
*/

#include <stdlib.h>    
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define MAXSIZE 100
int main(int argc , char **argv)
{
    int clientSocket;
    struct sockaddr_in server;

	  /* first - check to make sure there are 3 command line parameters
     (argc=4 since the program name is argv[0]) 
  */

		if (argc!=4) {
		  printf("Usage: client <IP address> <server port> <filename>\n");
		  exit(0);
		}

		int l;
		char *originalFile = argv[3]; 
    char message[MAXSIZE];
		char newName[MAXSIZE];
		char line[200];
		char server_reply[2000];

    /* create a socket 
     address family (AF_INET) 
     TCP protocol (SOCK_STREAM)
  	*/
    clientSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (clientSocket == -1)
    {
        printf("Problem creating socket");
				exit(1);
    }

		/* fill in an address structure that will be used to specify
			 the address of the server we want to connect to

			 address family is IP  (AF_INET)

			 server IP address is found by calling gethostbyname with the
			 name of the server (entered on the command line)

			 server port number is argv[2] (entered on the command line)
  	*/
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));

  	/* attempt to establish a connection with the server */
    if (connect(clientSocket , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        printf("Problem connecting\n");
        exit(1);
    }

    printf("Connected\n");

		/* Send the name of an existing ASCII file from client to server */
		l = strlen(originalFile) + 1;
		send(clientSocket, originalFile, l, 0);

		/* Client waits to receive ACK message and file name that will be written to from server before sending data */
		recv(clientSocket,message,MAXSIZE,0);	

		//don't proceed without receiving an Ack from server.
		if(strcmp(message,"Server Ack") != 0){
			printf("Error: Server Ack not received");
			exit(1);
		}
		recv(clientSocket,newName,MAXSIZE,0);
		printf("\nReceived newly created file name: %s \n", newName); //delete this


		//Client reads original file one line at a time, sends  to server

		FILE* file = fopen(originalFile, "r"); 
		while(fgets(line, sizeof(line), file)){
			//send a line
			send(clientSocket,line,MAXSIZE,0);

			memset(message,0,strlen(message));

			//don't proceed without receiving an Ack from server.
			recv(clientSocket,message,MAXSIZE,0);	
			if(strcmp(message,"Server Ack") != 0){
				printf("Error: Server Ack not received");
				exit(1);
			}
		}
		//concatenate '|' to end of message
		strcpy(line,"|");
		send(clientSocket, line, MAXSIZE, 0);
		fclose(file);
    close(clientSocket);
    return 0;
}



















