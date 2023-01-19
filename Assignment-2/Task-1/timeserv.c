// A Simple UDP Server that sends a HELLO message
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <time.h>

  
#define MAXLINE 100

// Gets the current time from the machine and returns it as a string
char *getTime(){
    time_t sys_time = time(NULL);
    char *time_string = ctime(&sys_time);
    time_string[strlen(time_string)-1] = '\0';
    return time_string;
}
  
int main() { 
    int sockfd; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Create socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( sockfd < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET; 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(8181); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    printf("\nServer Running....\n");
  
    int count;
    socklen_t len;
    char buffer[MAXLINE]; 
 
    len = sizeof(cliaddr);

    while(1){
        count = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, 
                ( struct sockaddr *) &cliaddr, &len);           // Receive HELLO from client   
        if(count<0){
            perror("Error While Receiving");
            exit(EXIT_FAILURE);
        }

        char currtime[MAXLINE];      
        strcpy(currtime, getTime());                            // Get current time from machine
        count = sendto(sockfd, currtime, strlen(currtime)+1, 0, // send current time to client
        (const struct sockaddr *) &cliaddr, sizeof(cliaddr)); 
        if(count<0){
            perror("Error While Sending");
            exit(EXIT_FAILURE);
        }
    }


    close(sockfd);
    return 0; 
} 