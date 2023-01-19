#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// dynamic concatenation
char* dynCat(char *st1, char *st2){
    char *temp = st1;
    
    st1 = (char *)malloc((strlen(st1)+strlen(st2))*sizeof(char));
    int n = strlen(temp), m = strlen(st2);
    for(int i=0; i<n; ++i){
        st1[i] = temp[i];
    }
    for(int j=0; j<m; ++j){
        st1[n+j] = st2[j];
    }

    return st1;
}

int add(int a, int b){
    return a+b;
}


#define SIZ 3
int main(){
    // char *st = "\0";
    // st = dynCat(st, "hdua");
    // st = dynCat(st, "GM");
    // st = dynCat(st, "_Rudrak");
    // printf("%s\n", st);
    char st[SIZ];
    scanf("%s", st);
    printf("%ld\n --> %s\n", strlen(st), st);


    return 0;
}