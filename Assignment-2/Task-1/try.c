#include <stdio.h>
#include <poll.h>

int main(){
    char buf[100];
    printf("Enter a string: ");
    fflush(stdout);

    struct pollfd fdset[1];
    fdset[0].fd = 0;
    fdset[0].events = POLLIN;
    
    int count=0;
    while(1){
        if(poll(fdset, 1, 1000)==1){
            scanf("%s", buf);
            break;
        }
        count++;
    }
    printf("You entered: %s\nTime taken = %d ms\n", buf, count*1000);
    return 0;
}