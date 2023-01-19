#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// BUF_LEN must be >= 3, so that the terminating expression "-1\n" can be read together 
#define BUF_LEN 7

int main()
{
	int	sockfd;
	struct sockaddr_in	serv_addr;
	char buf[BUF_LEN+1];	// +1 to store the '\0' inserted by fgets(), this '\0' is not sent to the server

	// Opening a new socket is exactly similar to the server process 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}


	serv_addr.sin_family = AF_INET;		// the internet family
	inet_aton("127.0.0.1", &serv_addr.sin_addr);	// 127.0.0.1 is a special address for "localhost" (this machine)
	serv_addr.sin_port	= htons(20000);	// specifies the port number of the server


	// connect to the server
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0) {		// status = -1 implies error while connecting
		perror("Unable to connect to server\n");
		exit(0);
	}

	// this flag helps to terminate the program when -1 is received
	int terminate_flag = 0;
	while(1){
		printf("Enter the Expression: ");

		// Flag which helps to terminate the program if "-1\n" is received as input 
		int first_input_flag=1;
		while(1){
			fgets(buf, BUF_LEN + 1, stdin);	// fgets() inserts a '\0' at the end so we put a +1 here

			// Checks if the entered expression is -1
			if(first_input_flag){
				first_input_flag = 0;
				if(buf[0]=='-' && buf[1]=='1' && buf[2]=='\n'){
					terminate_flag = 1;
					break;
				}
			}

			// flag to detect the end of expression
			int break_flag = 0;
			for(int i=0; i<BUF_LEN; ++i){
				if(buf[i]=='\n'){
					buf[i]='\0';
					break_flag=1;
					break;
				}
			}

			int count = send(sockfd, buf, strlen(buf), 0); // the +1 after strlen(buf) is ommited to not send '\0'
			if(count<0){
				perror("Error while sending\n");
				exit(0);
			}

			if(break_flag){
				send(sockfd, "\0", 1, 0);
				break;
			}
		}

		if(terminate_flag) break;
		
		// Receive the value of expression form server and print it
		char answer[35];		// 35 is a valid upperbound for length of any double 
		recv(sockfd, answer, 35*sizeof(char), 0);
		printf("Value of the expression = %s\n\n", answer);
		
	}

	close(sockfd);		// close the socket

	return 0;

}

