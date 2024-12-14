#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    
    //TODO The process needs to get the remaining time from somewhere
    remainingtime = atoi(argv[3]);
    printf("The process with id %s started at %d\n", argv[1], getClk());
    while (remainingtime > 0)
    {
        // remainingtime = ??;
        sleep(1);
        remainingtime--;
    }

    printf("The process with id %s finished at %d\n", argv[1], getClk());
    kill(getppid(), SIGUSR1);
    destroyClk(false);

    return 0;
}
