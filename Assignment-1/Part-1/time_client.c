#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
	int	sockfd ;
	struct sockaddr_in	serv_addr;

	int i;
	char buf[100];

	// Opening a new socket is exactly similar to the server process 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}


	serv_addr.sin_family = AF_INET;		// the internet family
	inet_aton("10.147.202.57", &serv_addr.sin_addr);	// 127.0.0.1 is a special address for "localhost" (this machine)
	serv_addr.sin_port	= htons(20000);	// specifies the port number of the server

	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/

	int status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	for(i=0; i < 100; i++) buf[i] = '\0';

	recv(sockfd, buf, 100, 0);		// Receive current time from server
	printf("Time Received: %s\n", buf);

	
	close(sockfd);		// close the socket
	return 0;

}

