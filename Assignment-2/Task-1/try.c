#include <stdio.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>

int main(){
    char buf[100];
    printf("Enter a string: ");
    fflush(stdout);

    struct pollfd fdset[1];
    fdset[0].fd = 0;
    fdset[0].events = POLLIN;
    
    int count=0;
    clock_t before; time(&before);

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    while(1){
        if(poll(fdset, 1, 3000)==1){
            scanf("%s", buf);
            break;
        }
        count++;
    }
    clock_t after; time(&after);
    int msec = difftime(after, before)*1000;
    printf("You entered: %s\nTime taken = %d ms\n", buf, count*3000);
    printf("More precise time taken = %dms %ld %ld\n", msec, before, after);

    gettimeofday(&stop, NULL);
    int time_taken = (stop.tv_sec - start.tv_sec)*1000 + (stop.tv_usec - start.tv_usec)/1000;
    printf("took %d ms\n", time_taken); 

    return 0;
}

// int main(){
//     int msec = 0, trigger = 1000, iterations=0; /* 10ms */
//     clock_t before = clock();

//     do {
//     /*
//     * Do something to busy the CPU just here while you drink a coffee
//     * Be sure this code will not take more than `trigger` ms
//     */

//     clock_t difference = clock() - before;
//     msec = difference * 1000 / CLOCKS_PER_SEC;
//     iterations++;
//     } while ( msec < trigger );

//     printf("Time taken %d seconds %d milliseconds (%d iterations)\n",
//     msec/1000, msec%1000, iterations);
//     return 0;
// }