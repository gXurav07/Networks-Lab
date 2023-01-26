#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
#define MIN_LOAD 1
#define MAX_LOAD 100

// returns dummy load
int dummy_load()
{
    int load = rand()%(MAX_LOAD - MIN_LOAD + 1) + MIN_LOAD;
    return load;
}

int main(){
    char buf[10];
    int x = dummy_load();
    sprintf(buf, "%d", x);
    printf("load=%d\n", x);
    printf("load=%s\n", buf);
    
    return 0;
}