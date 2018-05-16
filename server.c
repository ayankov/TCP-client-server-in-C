/*
*	Alexei Yankovsky
* CPSC 3600 HW4
* Date due: Nov 21, 2017
* Instructor: Feng Luo
*
* This is the threaded concurrent TCP server program. It uses threads to maintain concurrency.
* The following must be passed in on the command line:
*			Port number (argv[1])
*/

#include <time.h>
#include <stdio.h>
#include <string.h>  
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAXSIZE 100
#define ACK "Server Ack"


//Called connection to handle every new client
void *connection(void *socketDescriptor)
{
    //Get the socket descriptor
    int welcomeSocket = *(int*) socketDescriptor;
		char originalFile[MAXSIZE];
		char newName[MAXSIZE];
		char line[200];
		int idHash;

		recv(welcomeSocket,originalFile, MAXSIZE, 0);

		srand(time(NULL));
	
		sprintf(newName, "%s_client", originalFile);

		FILE *fptr;
		/* server creates new ASCII file for writing */
		fptr = fopen(newName,"w");

		/* server sends Ack message along with new file name to client */
		send(welcomeSocket, ACK, MAXSIZE, 0);		
		send(welcomeSocket, newName, MAXSIZE, 0);

			//printf("\nWrite line by line from client:\n");
			/* server reads ASCII data line by line sent by the client. Reverses order of chars in each line and writes to file opened by server */
		int i,j;
		for(i = 0; i<200; i++){
				/* Write a single line to new file: */
				//Receive the line
				recv(welcomeSocket,line, MAXSIZE,0);
				int c = 0;

				//check if it is the last line
				if(line[0] == '|')
					break;

				//Find length of line
				while(line[c] != '\0'){
					c= c+1;
				}
				//Write the line to new file
				for (j = c; j >= 1; j--){	
					//printf("%c", line[j]);
					fprintf(fptr,"%c",line[j]);
				}
				//Send Ack message to client after reading a line
				send(welcomeSocket, ACK, MAXSIZE, 0);
		}
		fclose(fptr);			
    free(socketDescriptor);
    close(welcomeSocket);
    pthread_exit(NULL); 
    return;
}




int main(int argc , char **argv)
{

		/* first - check to make sure there is 1 command line parameter
		 (argc=2 since the program name is argv[0]) 
		*/
		if (argc!=2) {
			printf("Usage: server <server port>\n");
			exit(0);
		}

		int filenum = 0;
    int socketDescriptor;
		int clientSocket;
		int *newSocket;
		int c;

		struct sockaddr_in client; //server sends to client on this address
    struct sockaddr_in server; //server receives on this address

    /* create a socket 
       address family (AF_INET) 
       TCP protocol (SOCK_STREAM)
		*/
    socketDescriptor = socket(AF_INET , SOCK_STREAM , 0);
    if (socketDescriptor == -1)
    {
        perror("Problem creating socket.\n");
				exit(1);
    }


    /* establish our address 
			 address family is AF_INET
			 our IP address is INADDR_ANY (any of our IP addresses)
			 the port number is assigned by the kernel 
		*/
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

		//bind
    if( bind(socketDescriptor,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Problem binding");
        exit(1);
    }


    /* put the socket into passive mode (waiting for connections) */
    if(listen(socketDescriptor,5) < 0){ //max 5 connections
				perror("Error calling listen\n");
    		exit(1);	
		}
		printf("Waiting for a client...\n");


    c = sizeof(struct sockaddr_in);
		/* now process incoming connections ... */
    while( (clientSocket = accept(socketDescriptor, (struct sockaddr *)&client, (socklen_t*)&c)))
    {
        printf("\nIncoming connection accepted.\n");

        pthread_t sniffer_thread;
        newSocket = malloc(sizeof *newSocket);
        *newSocket = clientSocket;

        if( pthread_create( &sniffer_thread , NULL ,  connection , (void*) newSocket) < 0)
        {
            perror("Problem creating thread");
            exit(1);
        }

    }

    if (clientSocket < 0)
    {
        perror("accept failed");
        exit(1);
    }

    return 0;
}



















