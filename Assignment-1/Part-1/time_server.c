#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>


// Gets the current time from the machine and returns it as a string
char *getTime(){
    time_t sys_time = time(NULL);
    char *time_string = ctime(&sys_time);
    time_string[strlen(time_string)-1] = '\0';
    return time_string;
}


int main(){
	int	sockfd, newsockfd;		// socket descriptors 
	int	clilen;
	struct sockaddr_in	cli_addr, serv_addr;
	

	int i, status;				// status variable for bind() function
	char curr_time[100];		// used to communicate with client 		

	// Opens a socket (returns -1 incase an error occurs )
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

	serv_addr.sin_family = AF_INET;			// the internet family
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// set to INADDR_ANY for machines having a single IP address
	serv_addr.sin_port = htons(20000);		// specifies the port number of the server


	// bind() function to provide local address to the socket
	status = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status< 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	// Specifies that upto 5 active clients can wait for a connection
	listen(sockfd, 5); 

	while(1) {
		// accept() function waits until a client connects to the server and then sending/receiving pf data occurs
		// newsockfd is used for communicating with client
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 

		// if erroe in accept() then -1 is returned
		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}
		
		strcpy(curr_time, getTime());	// get the current machine time in curr_time variable
		send(newsockfd, curr_time, strlen(curr_time) + 1, 0);	// send the current time
		close(newsockfd);
	}
	return 0;
}
			

