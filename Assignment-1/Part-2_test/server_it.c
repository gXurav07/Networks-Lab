#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>


// dynamic concatenation function
char* dynCat(char *st1, char *st2){
    char *temp;
    int n = strlen(st1), m = strlen(st2);
    temp = (char *)malloc((n+m+1)*sizeof(char)); // Allocates space for the new string

    for(int i=0; i<n; ++i){
        temp[i] = st1[i];
    }
    for(int j=0; j<m; ++j){
        temp[n+j] = st2[j];
    }
	free(st1);
    temp[n+m]='\0';     // make last character '\0
    //printf("%s", temp);
    return temp;
}

float apply(float arg1, float arg2, char op){
	if(op=='+') return arg1+arg2;
	if(op=='-') return arg1-arg2;
	if(op=='*') return arg1*arg2;
	return arg1/arg2;
}


// Assuming no nested paranthesis
float evaluate(char *exp){
	// Since, nested paranthesis is not present, max stack size needed is 2
	float numStack[2]={0, 0};		// stack for operands or number
	char opStack[2] = {'\0', '\0'};		// stack for operators
	int stkPos=0; 			// specifies which position in stack to use. (use stkPos 1 if inside paranthesis, 0 if outside)

	int n = strlen(exp), numLen = 0;
	char num[100]; // 100 is a valid upperbound for length of a float
	num[0]='\0';

	for(int i=0; i<n; ++i){
	
		char ch = exp[i];

		if(ch==' ') continue;

		if(ch=='('){
			stkPos = 1;
			continue;
		}

		if(((ch-'0'>9)||(ch-'0'<0))&&(ch!='.')){			// if we receive non-digit character
			if(opStack[stkPos]!='\0'){
				numStack[stkPos] = apply(numStack[stkPos], atof(num), opStack[stkPos]);  
				opStack[stkPos]='\0';
			}
			else {
				numStack[stkPos] += atof(num); 
			}
			num[0] = '\0';
			numLen = 0;
		}						// if we receive a digit or .
		else{
			num[numLen] = ch;
			num[numLen+1]= '\0';
			numLen++;
		}




		if(ch == ')'){
			if(opStack[0]!='\0'){
				numStack[0] = apply(numStack[0], numStack[1], opStack[0]); 
				opStack[0] = '\0';
			}
			else {
				numStack[0] += numStack[1]; 
			}

			num[0] = '\0';
			numLen = 0;
			opStack[1]='\0';
			numStack[1]=0;
			stkPos=0;		// change stack pointer position
			
		}
		if(ch=='+' || ch=='-' || ch=='*' || ch=='/'){
			opStack[stkPos] = ch;
		}


	}
	

	if(opStack[stkPos]!='\0'){
		numStack[stkPos] = apply(numStack[stkPos], atof(num), opStack[stkPos]);
	}
	else {
		numStack[stkPos] += atof(num); 
	}


	return numStack[stkPos];
}


int main(){
	int	sockfd, newsockfd;		// socket descriptors 
	int	clilen;
	struct sockaddr_in	cli_addr, serv_addr;
	

	int i, status, bufLen=5;				// status variable for bind() function
	char buf[bufLen];		        // used to communicate with client 
    char *expression = (char *)malloc(sizeof(char));  // stores the expression sent by the client	
	expression[0] = '\0';
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
		

        // Receive the expression from client
		
        while(1){

            int count = recv(newsockfd, buf, bufLen*sizeof(char), 0);
			printf("count=%d\n", count);
            if(count<0){
                perror("Error while receiving\n");
                exit(0);
            }

			printf("ASCII:- ");
			for(int i=0;i<bufLen; ++i){
				printf("%d, ", buf[i]-'\0');
			}
			printf("\n");


			if(buf[0]=='\0') break;
			
            expression = dynCat(expression, buf);
			printf("%s\n\n", buf);
          
			
        }
        
        printf("Expression Received: %s\n", expression);
		float answer = evaluate(expression);
		printf("Answer=%f\n", answer);


		char strAns[50]; 
		gcvt(answer, 50, strAns);	// converts float to string
		send(newsockfd, strAns, 50*sizeof(char), 0);

		expression[0]='\0';
		
		close(newsockfd);
	}
	return 0;
}
			

