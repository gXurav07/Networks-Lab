#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 60

int min( int a, int b){
    if(a<b) return a;
    return b;
}

void receive_in_packets(int sockfd, char *buf, int size){
    const int PACKET_SIZE = 4;
    int bytes_received = 0;
    buf[0] = '\0';
    while(bytes_received < size){
        int bytes = recv(sockfd, buf + bytes_received, min(size - bytes_received, PACKET_SIZE), 0);
        if(bytes == -1){
            perror("Error in receiving data");
            exit(0);
        }
        if(bytes == 0){
            break;
        }
        bytes_received += bytes;
        if(buf[bytes_received-1] == '\0'){
            break;
        }
    }
}


int main(int argc, char **argv)
{
	int	sockfd;
	struct sockaddr_in	serv_addr;
    int lb_port_no;

	int i;
	char buf[BUF_SIZE];

    // Load Balancer port number given as input
    lb_port_no = atoi(argv[1]);

	// Opening a new socket is exactly similar to the server process 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}


	serv_addr.sin_family = AF_INET;		// the internet family
	inet_aton("127.0.0.1", &serv_addr.sin_addr);	// 127.0.0.1 is a special address for "localhost" (this machine)
	serv_addr.sin_port	= htons(lb_port_no);	// specifies the port number of the server

	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/

	int status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	for(i=0; i < BUF_SIZE; i++) buf[i] = '\0';

	receive_in_packets(sockfd, buf, BUF_SIZE);
	printf("Time Received: %s\n", buf);

	
	close(sockfd);		// close the socket
	return 0;

}
