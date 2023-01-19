#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


void wait()
{
    // waits for 1000 milliseconds
    int milli_seconds = 1000; 
 
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}


int main()
{
	int	sockfd;
	struct sockaddr_in	serv_addr;

	int i;
	char buf[100];


	
	// Opening a new socket is exactly similar to the server process 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}


	serv_addr.sin_family = AF_INET;		// the internet family
	inet_aton("127.0.0.1", &serv_addr.sin_addr);	// 127.0.0.1 is a special address for "localhost" (this machine)
	serv_addr.sin_port	= htons(20000);	// specifies the port number of the server

	int status = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	while(1){
		printf("Enter the Expression: ");
		

		for(i=0; i < 100; i++) buf[i] = '\0';

		
		while(1){
			fgets(buf, 7, stdin);
			int break_flag = 0;
			for(int i=0; i<6; ++i){
				if(buf[i]=='\n'){
					buf[i]='\0';
					break_flag=1;
					break;
				}
			}

			printf("%s  --  %ld\n", buf, strlen(buf));
			int count = send(sockfd, buf, strlen(buf), 0); // the +1after strlen(buf) is ommited to not send '\0'
			if(count<0){
				perror("Error while sending\n");
				exit(0);
			}

			wait();

			if(break_flag){
				buf[0]='\0';
				send(sockfd, buf, strlen(buf)+1, 0);
				printf("Ended!\n");
				break;
			}
		}
		
		printf("Waiting for answer!\n");
		recv(sockfd, buf, 50*sizeof(char), 0);
		printf("Value of the expression = %s\n", buf);
		
		
		

		// Ask user if he/she wants to continue
		printf("Enter 1 to CONTINUE and 0 to END: ");
		int choice_to_continue;
		scanf("%d", &choice_to_continue);

		if(!choice_to_continue) break;
		getchar();		// Helps to avoid input problems due to '\n' at the end of previous input
	}

	
	send(sockfd, "-1\0", 3, 0);
	close(sockfd);		// close the socket

	return 0;

}

