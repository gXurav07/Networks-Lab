#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <time.h>

#define BUF_SIZE 50

int min( int a, int b){
    if(a<b) return a;
    return b;
}

void receive_in_packets(int sockfd, char *buf, int size); // receives data in packets
int get_sockfd(int port_no);    // opens a new socket returns its descriptor (doesn't binds if port_no = -1)
struct sockaddr_in get_serv_addr(int port_no, char *ip_addr); // returns a server address
int get_load(struct sockaddr_in serv_addr); // gets load of server
int wait_for_client(int sockfd_cli, int time_in_sec); // returns newsockfd if connected to client, -1 otherwise 
void get_send_time(int sockfd, struct sockaddr_in serv_addr); // gets time from server and sends it to client


int main(int argc, char **argv)
{
    int port_lb, port_s1, port_s2; // port numbers of load balancer, server 1 and server 2
	struct sockaddr_in	serv_addr_1, serv_addr_2; // server addresses

	int	sockfd_cli, newsockfd;			// socket descriptors
	int load_1, load_2, timeout, diff; 	// load of servers, timeout for poll() in sec
	clock_t before, after;				// for timeout

	if(argc != 4){
		printf("Usage: ./lb <port_lb> <port_s1> <port_s2>\n");
		exit(0);
	}
		

	port_lb = atoi(argv[1]);
	port_s1 = atoi(argv[2]);
	port_s2 = atoi(argv[3]);

	
	sockfd_cli = get_sockfd(port_lb);		// socket to receive client calls (binded)
    printf("\nLOAD BALANCER started!\n"); 
	printf("Listening on port number %d .......\n\n", port_lb);fflush(stdout);
    
	serv_addr_1 = get_serv_addr(port_s1, "127.0.0.1"); // get address of server 1
	serv_addr_2 = get_serv_addr(port_s2, "127.0.0.1"); // get address of server 1


	load_1 = get_load(serv_addr_1);
	load_2 = get_load(serv_addr_2);
	printf("\n"); 
	timeout = 5; // in sec
	while(1){
		time(&before);
		newsockfd = wait_for_client(sockfd_cli, timeout);	// returns -1 if no client connected
		time(&after);
    
		if(newsockfd == -1){ 
			load_1 = get_load(serv_addr_1);
			load_2 = get_load(serv_addr_2); printf("\n"); 
			timeout=5; // in sec
			continue;
		}
		
		if(fork() != 0){ // parent process
			close(newsockfd);
			diff = difftime(after, before); // in sec
			timeout = timeout - diff; 
			continue;
		}
		
		// child process
		close(sockfd_cli); 
		if(load_1<load_2)
			get_send_time(newsockfd, serv_addr_1);
		else
			get_send_time(newsockfd, serv_addr_2);
		exit(0);

	}

	close(sockfd_cli);
	return 0;

}

// receives a buffer in chunks of size BUF_SIZE
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


// returns a socket descriptor (doesn't binds if port_no = -1)
int get_sockfd(int port_no){
	int	sockfd;		// socket descriptors 
	int status;		// status variable for bind() function	

	// Opens a socket (returns -1 incase an error occurs )
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

    if(port_no<0) 
        return sockfd;    // Don't bind incase -1 is received

    struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;			// the internet family
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// set to INADDR_ANY for machines having a single IP address
	serv_addr.sin_port = htons(port_no);	// specifies the port number of the server


	// bind() function to provide local address to the socket
	status = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status< 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	// Specifies that upto 5 active clients can wait for a connection
	listen(sockfd, 5);

    return sockfd;
}

struct sockaddr_in get_serv_addr(int port_no, char *ip_addr){
    struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;		// the internet family
	inet_aton(ip_addr, &serv_addr.sin_addr);	
	serv_addr.sin_port	= htons(port_no);	// specifies the port number of the server


    return serv_addr;
}

// gets load of server
int get_load(struct sockaddr_in	serv_addr){
	int sockfd_serv = get_sockfd(-1);           // socket to sommunicate with server (unbinded)
	int status = connect(sockfd_serv, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	char buf[BUF_SIZE]; // bufffer to communicate with server
	for(int i=0; i < BUF_SIZE; i++) buf[i] = '\0';

    strcpy(buf, "Send Load");
	send(sockfd_serv, buf, strlen(buf)+1, 0);
	receive_in_packets(sockfd_serv, buf, BUF_SIZE);
    

    printf("Load Received from %s ", inet_ntoa(serv_addr.sin_addr));
	printf("%s\n", buf); fflush(stdout);
	close(sockfd_serv);		// close the socket
	
	return atoi(buf);


}

// returns newsockfd if connected to client, -1 otherwise 
int wait_for_client(int sockfd_cli, int time_in_sec){
		struct sockaddr_in cli_addr;
		int newsockfd, clilen=sizeof(cli_addr), flag=0;

		// Specify POLL fdset and events
		struct pollfd fdset[1];
		fdset[0].fd = sockfd_cli;
		fdset[0].events = POLLIN;

		if(poll(fdset, 1, time_in_sec*1000)==1){
			newsockfd = accept(sockfd_cli, (struct sockaddr *) &cli_addr, &clilen); 

			// if error in accept() then -1 is returned
			if (newsockfd < 0) {
				perror("Accept error\n");
				exit(0);
			}
			else
				flag=1;
		}

		if(flag)
			return newsockfd;
		else
			return -1;

}

// gets time from server and sends it to client
void get_send_time(int sockfd, struct sockaddr_in serv_addr){
	printf("Sending client request to Server %s \n\n", inet_ntoa(serv_addr.sin_addr)); fflush(stdout);
	int sockfd_serv = get_sockfd(-1);           // socket to sommunicate with server (unbinded)
	int status = connect(sockfd_serv, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	char buf[BUF_SIZE]; // bufffer to communicate with server
	for(int i=0; i < BUF_SIZE; i++) buf[i] = '\0';

	strcpy(buf, "Send Time");
	send(sockfd_serv, buf, strlen(buf)+1, 0);
	receive_in_packets(sockfd_serv, buf, BUF_SIZE);
	close(sockfd_serv);		// close the socket

	send(sockfd, buf, strlen(buf)+1, 0);
	close(sockfd);		// close the socket
}
