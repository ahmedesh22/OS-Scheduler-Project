#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
int startTime;
int main(int agrc, char *argv[])
{
    initClk();
    remainingtime = atoi(argv[3]);
    startTime=getClk();
    //printf("The process with id %s started at %d\n", argv[1], startTime);
    while (remainingtime > 0)
    {
        if(getClk() - startTime >= 1){
        remainingtime--;
        startTime=getClk();
        //printf("The process with id %s is running at %d\n", argv[1], getClk());
        }
    }

    //printf("The process with id %s finished(or stopped) at %d\n", argv[1], getClk());
    kill(getppid(), SIGUSR1);
    destroyClk(false);
    return 0;
}
