// CPP program to evaluate a given
// expression where tokens are
// separated by space.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// void wait()
// {
//     // waits for 1000 milliseconds
//     int milli_seconds = 1000; 
 
//     // Storing start time
//     clock_t start_time = clock();
 
//     // looping till required time is not achieved
//     while (clock() < start_time + milli_seconds)
//         ;
// }

// Performs binary arithmetic operations 
double apply(double arg1, double arg2, char op){
	if(op=='+') return arg1+arg2;
	if(op=='-') return arg1-arg2;
	if(op=='*') return arg1*arg2;
	return arg1/arg2;
}


// GLOBAL VARIABLES:-	
// Since, nested parenthesis is not present, max stack size needed is 2
double numStack[2];		// stack for operands/numbers
char opStack[2];		// stack for operators
int stkPos; 			// specifies which position in stack to use. (use stkPos 1 if inside parenthesis, 0 if outside)
int numLen;				// stores length of operand stored in num 
char num[60]; 			// 30 is a valid upperbound for length of a double

void initialize_globals(){
	numStack[0]=0; numStack[1]=0;
	opStack[0]='\0'; opStack[1]='\0';
	stkPos=0;
	numLen=0;
	num[0]='\0';
}

// Evaluate Function (Assuming no nested parenthesis)
double evaluate(char *exp){
    int n = strlen(exp);

	for(int i=0; i<n; ++i){
		char ch = exp[i];
		if(ch==' ') continue;	// ignore white spaces
		if(ch=='('){			// change stkPos to 1 on moving inside parenthesis
			stkPos = 1;
			continue;
		}
		if(((ch-'0'>9)||(ch-'0'<0))&&(ch!='.')){			// if we receive non-digit character (operators or parenthesis)
			if(opStack[stkPos]!='\0'){
				numStack[stkPos] = apply(numStack[stkPos], atof(num), opStack[stkPos]);  
				opStack[stkPos]='\0';
			}
			else {
				numStack[stkPos] += atof(num); 
			}
			num[0] = '\0';
			numLen = 0;
		}						
		else{			// if we receive a digit or .
			num[numLen] = ch;
			num[numLen+1]= '\0';
			numLen++;
		}

		if(ch == ')'){		// Bring out the result calculated inside parenthesis 
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
	// Final calculation on reseiving exp = "\0"
    
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
    
	return numStack[0];
}
int main() {
	// cout << evaluate("10 + 2 * 6") << "\n";
	// cout << evaluate("100 * 2 + 12") << "\n";
	// cout << evaluate("100 * ( 2 + 12 )") << "\n";

	char *exp = "5*(-2)";
	printf("%f\n", evaluate(exp));
	return 0;
}

// This code is contributed by Nikhil jindal.
