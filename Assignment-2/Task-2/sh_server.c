#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h> 


#define BUFF_SIZE 300


// receives a string in multiple packets
void bigReceive(int newsockfd, char *buf){
    const int maxReceiveSize = 5;
    int i=0;
    while(1){
        int size = maxReceiveSize;
        int count = recv(newsockfd, buf, size, 0);
        if(count==0){                   // client closed connection
            buf[0]='\0';
            return;
        }
        if(buf[count-1]=='\0') break;   // command finished
        buf+=count;
    }
    return;
}

// sends a string in multiple packets
void bigSendPro(char* buf, int sockfd, int n){ // n = number of characters to send
    printf("\nSending:-\n");
    char xx[200];
    strcpy(xx, buf);
    xx[n]='\0';
    printf("%d, %s\n", n, xx);

	const int maxSendSize = 7;
	int i=0;
	while(i<n){
		int size = maxSendSize;
		if(i+maxSendSize>n) size = n-i;
		send(sockfd, buf+i, size, 0);
		i+=maxSendSize;
	}
    printf("[Done]\n\n");
	return;
}



void execute(char *command, int sockfd){
    if(command[0]=='c' && command[1]=='d'){     // execute cd 
        if(strlen(command)==2 || command[2]!=' '){
            bigSendPro("####", sockfd, 5); ////////
        }
        else{
            char *path;
            path = command+3;
            int ch=chdir(path);
            if(ch<0) bigSendPro("####", sockfd, 5);
            else bigSendPro("\0", sockfd, 1);
        }
    }
    else if(command[0]=='p' && command[1]=='w' && command[2]=='d' && strlen(command)==3){  // execute pwd
        char output[210];
        char *status = getcwd(output, 205*sizeof(char));

        if(status==NULL) bigSendPro("####", sockfd, 5);
        else{
            bigSendPro(output, sockfd, strlen(output)+1);
        }
    }
    else if(command[0]=='d' && command[1]=='i' && command[2]=='r'){  // execute dir
        char *path;
        if(strlen(command)==3) path = ".";
        else if(command[3]!=' '){
            bigSendPro("$$$$", sockfd, 5);
            return;
        }
        else path = command+4;

        DIR *dir = opendir(path);
        if(dir==NULL) bigSendPro("####", sockfd, 5);
        else{
            struct dirent *entry;
            while((entry = readdir(dir))!=NULL){
                if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0) continue;  
                bigSendPro(entry->d_name, sockfd, strlen(entry->d_name));
                bigSendPro("\n", sockfd, 1);
            }  
            bigSendPro("\0", sockfd, 1); 
        }
        

    }
    else bigSendPro("$$$$", sockfd, 5);
}

int main(){
	int	sockfd, newsockfd;		// socket descriptors 
	int	clilen;
	struct sockaddr_in	cli_addr, serv_addr;
	

	int status, count;				// status variable for bind() function
	char buff[BUFF_SIZE];		// used to communicate with client 		

	// Opens a socket (returns -1 incase an error occurs )
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

	serv_addr.sin_family = AF_INET;			// the internet family
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// set to INADDR_ANY for machines having a single IP address
	serv_addr.sin_port = htons(20000);		// specifies the port number of the server

    clilen = sizeof(cli_addr);

	// bind() function to provide local address to the socket
	status = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status< 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	// Specifies that upto 5 active clients can wait for a connection
	listen(sockfd, 5); 
	printf("Server Running...\n");
    

	while(1) {

		
		// newsockfd is used for communicating with client
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 

		// if error in accept() then -1 is returned
		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}
		
		strcpy(buff, "LOGIN:");                         	// put "LOGIN:" in buff
		send(newsockfd, buff, strlen(buff), 0);	            // send "LOGIN:" to client

        
        count = recv(newsockfd, buff, BUFF_SIZE, 0);        // receive the username from client
        printf("Received username: %s\n", buff);                               // print the received message

        
        // Search for the username in users.txt

        FILE *fp;
        fp = fopen("users.txt", "r");
        char line[100];
        int flag = 0;
        while(fscanf(fp, "%s", line) != EOF){
            //printf("%s||%s||\n", line, buff);
            if(strcmp(line, buff) ==0){
                flag = 1;
                break;
            }
        }
        fclose(fp);

        if(flag == 1){
            strcpy(buff, "FOUND");                              // put "FOUND" in buff
            send(newsockfd, buff, strlen(buff), 0);             // send "FOUND" to client
        }
        else{
            strcpy(buff, "NOT-FOUND");                          // put "NOT-FOUND" in buff
            send(newsockfd, buff, strlen(buff), 0);             // send "NOT-FOUND" to client
        }

        char *curr_dir = getcwd(NULL, 0);

		while(1){
			bigReceive(newsockfd, buff);		// receive a command from client
			if(strlen(buff)==0) break;
			printf("Received command: %s\n", buff);				// print the received message
			
	
			execute(buff, newsockfd);

		}

        chdir(curr_dir);
        free(curr_dir);
		close(newsockfd);
        
	}
    
	return 0;
}
			

