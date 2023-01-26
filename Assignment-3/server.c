#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>


#define BUF_SIZE 50
#define MIN_LOAD 1
#define MAX_LOAD 100


// returns a dummy load(random number)
int dummy_load()
{
    int load = rand()%(MAX_LOAD - MIN_LOAD + 1) + MIN_LOAD;
    return load;
}

// Gets the current time from the machine and returns it as a string
char *getTime(){
    time_t sys_time = time(NULL);
    char *time_string = ctime(&sys_time);
    time_string[strlen(time_string)-1] = '\0';
    return time_string;
}


void receive_in_packets(int sockfd, char *buf, int size){
    int bytes_received = 0;
    buf[0] = '\0';
    while(bytes_received < size){
        int bytes = recv(sockfd, buf + bytes_received, size - bytes_received, 0);
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


int main(int argc, char **argv){
    int port_no=atoi(argv[1]);
	int	sockfd, newsockfd;		// socket descriptors 
	int	clilen;
	struct sockaddr_in	cli_addr, serv_addr;
	

	int i, status, load;				// status variable for bind() function
	char buf[BUF_SIZE];		// used to communicate with client 		

	// Opens a socket (returns -1 incase an error occurs )
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

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
    printf("Server is listening on port %d ......", port_no); 
    fflush(stdout);

	while(1) {
		// accept() function waits until a client connects to the server and then sending/receiving pf data occurs
		// newsockfd is used for communicating with client
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 

		// if error in accept() then -1 is returned
		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}


        while(1){
            // receive the message from the client
            receive_in_packets(newsockfd, buf, BUF_SIZE);

            // check the received message
            if(strcmp(buf, "")==0) {break;} // if nothing was sent
            else if(strcmp(buf, "Send Time")==0){
                strcpy(buf, getTime());	// get the current machine time in buf variable
                send(newsockfd, buf, strlen(buf) + 1, 0);	// send the current time
            }
            else if(strcmp(buf, "Send Load")==0){
                load = dummy_load();
                sprintf(buf, "%d", load);	// get the load in buf variable
                send(newsockfd, buf, strlen(buf) + 1, 0);	// send the load
                printf("Load sent: %d\n", load);
            }
            else{
                printf("Invalid Request received!");
            }



        }
		close(newsockfd);
	}
	return 0;
}
			
