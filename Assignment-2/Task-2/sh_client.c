#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 210
#define MAX_USERNAME_SIZE 25

// sends a string in multiple packets
void bigSend(int sockfd,char* buf){
	const int maxSendSize = 7;
	int n = strlen(buf)+1;
	int i=0;
	while(i<n){
		int size = maxSendSize;
		if(i+maxSendSize>n) size = n-i;
		send(sockfd, buf+i, size, 0);
		i+=maxSendSize;
	}
	return;
}

// receives a string in multiple packets
void bigRecvPrint(int newsockfd){
    const int maxReceiveSize = 6;
    int i=0;
	char recv_buf[maxReceiveSize+1];
    while(1){
        int size = maxReceiveSize;
        int count = recv(newsockfd, recv_buf, size, 0);
		recv_buf[count]='\0';
		printf("%s", recv_buf);
        if(recv_buf[count-1]=='\0') break;   // command finished
        
    }
	printf("\n");
	fflush(stdout);
    return;
}

int main()
{
	int	sockfd ;
	struct sockaddr_in	serv_addr;

	int count, status;
	char buf[BUF_SIZE], curr_dir[100];

	// Opening a new socket is exactly similar to the server process 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}


	serv_addr.sin_family = AF_INET;		// the internet family
	inet_aton("127.0.0.1", &serv_addr.sin_addr);	// 127.0.0.1 is a special address for "localhost" (this machine)
	serv_addr.sin_port	= htons(20000);	// specifies the port number of the server

	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process. */

	status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
    else{
        printf("Connection Established....\n\n"); ////////////////
    }

	for(int i=0; i < BUF_SIZE; i++) buf[i] = '\0';

	count = recv(sockfd, buf, BUF_SIZE, 0);			// Receive "LOGIN:" from server
    buf[count]='\0';
	printf("%s ", buf);                           	// print the received message

    fgets(buf, MAX_USERNAME_SIZE+2, stdin);         // Take the username input, +2 for newline and null character   
	buf[strlen(buf)-1] = '\0';						// remove the trailing newline character 
	                   
    send(sockfd, buf, strlen(buf)+1, 0);           	// send the username to server

    count = recv(sockfd, buf, BUF_SIZE, 0);        	// receive the found/not-found message from server
    buf[count]='\0';
    
    if(strcmp("FOUND", buf)!=0){
        printf("Invalid username\n");
        close(sockfd);
        exit(0);
    }
    
	while(1){
		printf("Enter a command: "); 

		fgets(buf, BUF_SIZE, stdin);		// read the command from the user
		buf[strlen(buf)-1] = '\0';			// remove the trailing newline character
		

		if(strcmp(buf, "exit") == 0) break;	// if the command is "exit", break out of the loop

		bigSend(sockfd, buf);	// send the command to the server

		bigRecvPrint(sockfd);	// receive the output from the server and print it

	}
	

	
	close(sockfd);		// close the socket
	return 0;

}
