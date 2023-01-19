#include <stdio.h>
#include <string.h>

int main(){
    char buf[100];
    fgets(buf, 6, stdin); // max 5 characters + 1 for null
    //buf[strlen(buf)-1] = '\0';
    printf("%s, %d\n", buf, buf[5]-'\0');
    return 0;
}