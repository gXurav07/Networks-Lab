// A Simple Client Implementation
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <poll.h>

#define BUF_SIZE 120
#define TIMEOUT 3000
#define MAX_TRIES 5
  
int main() { 
    int sockfd; 
    struct sockaddr_in servaddr; 
    char buf[BUF_SIZE];
    int count;
  
    // Creating socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(8181); 
    inet_aton("127.0.0.1", &servaddr.sin_addr); 
      
    int n;
    socklen_t len; 
    char *hello = "CLIENT:HELLO"; 
      
    sendto(sockfd, (const char *)hello, strlen(hello), 0, 
			(const struct sockaddr *) &servaddr, sizeof(servaddr)); // Send HELLO to server
    
    
    struct pollfd fdset[1];
    fdset[0].fd = sockfd;
    fdset[0].events = POLLIN;


    int tries_left=MAX_TRIES, flag=0;
    while(tries_left--){
        if(poll(fdset, 1, TIMEOUT)==1){
            count = recvfrom(sockfd, (char *)buf, BUF_SIZE, 0, 
			    ( struct sockaddr *) &servaddr, &len); 
            flag = 1;
            break;
        }
    }

    if(!flag){
        printf("Timeout exceeded\n");
        exit(EXIT_FAILURE);
    }


    printf("Time received: %s\n", buf);



           
    close(sockfd); 
    return 0; 
} 