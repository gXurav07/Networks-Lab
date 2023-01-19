#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>

const int BUF_SIZE = 5;

double apply(double arg1, double arg2, char op){
	if(op=='+') return arg1+arg2;
	if(op=='-') return arg1-arg2;
	if(op=='*') return arg1*arg2;
	return arg1/arg2;
}


// All global variables 
// Since, nested paranthesis is not present, max stack size needed is 2
double numStack[2];		// stack for operands or number
char opStack[2];		// stack for operators
int stkPos; 			// specifies which position in stack to use. (use stkPos 1 if inside paranthesis, 0 if outside)
int numLen;
char num[30]; 			// 60 is a valid upperbound for length of a double

void initialize_globals(){
	numStack[0]=0; numStack[1]=0;
	opStack[0]='\0'; opStack[1]='\0';
	stkPos=0;
	numLen=0;
	num[0]='\0';
}

// Assuming no nested paranthesis
void evaluate(char *exp){
    int n = strlen(exp);
	for(int i=0; i<n; ++i){
	
		char ch = exp[i];

		if(ch==' ') continue;	// ignore white spaces

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

    if(strlen(exp)==0){
        if(opStack[0]!='\0'){
            numStack[0] = apply(numStack[0], atof(num), opStack[0]);
			opStack[0]='\0';
			numLen=0;
			num[0]='\0';
        }
        else {
            numStack[0] += atof(num); 
			numLen=0;
			num[0]='\0';
        }
    }


}


int main(){
	int	sockfd, newsockfd;		// socket descriptors 
	int	clilen;
	struct sockaddr_in	cli_addr, serv_addr;
	

	int i, status;				// status variable for bind() function
	char buf[BUF_SIZE];		        // used to communicate with client 
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

	while(1){
		// accept() function waits until a client connects to the server and then sending/receiving pf data occurs
		// newsockfd is used for communicating with client
		initialize_globals();
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 

		// if erroe in accept() then -1 is returned
		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}

		int disconnect_flag=0;
		while(1) {
			initialize_globals(); 	// initializes Global variables

			while(1){
				// Receive the expression from client
				int count = recv(newsockfd, buf, BUF_SIZE*sizeof(char), 0);

				if(count<0){
					perror("Error while receiving\n");
					exit(0);
				}

				if(count==0){
					disconnect_flag=1;
					break;
				}

				if(count<BUF_SIZE) buf[count]='\0';
				
				evaluate(buf);

				if(buf[count-1]=='\0') break;


			}

			if(disconnect_flag) break;

			evaluate("\0");
			
			
			double answer = numStack[0];		// get answer from stack

			
			char strAns[30]; // 30 is a valid upperbound for length of any double
			gcvt(answer, 30, strAns);	// converts double to string
			send(newsockfd, strAns, strlen(strAns)+1, 0);

			
		}
		close(newsockfd);
	}

	close(sockfd);		
	return 0;
}
			

