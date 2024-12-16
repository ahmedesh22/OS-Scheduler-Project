#pragma once
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
typedef short bool;
#define true 1
#define false 0
FILE *file;
FILE *file2;
float total_run=0;
float total_wait=0;
float total_WTA=0;
float lastfinish=0;
#define SHKEY 300

typedef enum P_state {
  WAITING,
  RUNNING,
  READY
} P_state ;

struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int starttime;
    int stoptime;
    int waittime;
    int id;
    int pid;
    int remainingTime;
    P_state state;
};



typedef enum Scheduling_Algorithm
{
    SJF=1,
    HPF,
    RR,
    MLFL
}Scheduling_Algorithm;




struct msgbuff
{
    long mtype;
    //char mtext[256];
    struct processData process;
};
///==============================
//don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All processes call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All processes call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
void write_output_file(struct processData* pd, int state) // States are Started (0), Finished(1), Stopped(2), Resumed(3) 
{
    if (state == 0) // Started
    {
        fprintf(file, "At time %d process %d started arr %d total %d remain %d wait %d\n", getClk(), 
        pd->id, pd->arrivaltime, pd->runningtime, pd->remainingTime, pd->waittime);
    }
    else if (state == 1) // Finsihed
    {
        lastfinish=getClk();
        total_run+=pd->runningtime;
        total_wait+=pd->waittime;
        total_WTA+=(float)(getClk() - pd->arrivaltime) / pd->runningtime;
        fprintf(file, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), 
        pd->id, pd->arrivaltime, pd->runningtime, pd->remainingTime, pd->waittime, getClk() - pd->arrivaltime, (float) (getClk() - pd->arrivaltime) / pd->runningtime);
    }
    else if (state == 2) // Stopped
    {
        fprintf(file, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), 
        pd->id, pd->arrivaltime, pd->runningtime, pd->remainingTime, pd->waittime);
    }
    else if (state == 3) // Resumed
    {
        fprintf(file, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", getClk(), 
        pd->id, pd->arrivaltime, pd->runningtime, pd->remainingTime, pd->waittime);
    }
    else if (state == 4) // still having cpu for another quantum in RR
    {
        fprintf(file, "At time %d process %d still having the cpu  remain %d wait %d\n", getClk(), 
        pd->id, pd->remainingTime, pd->waittime);
    }
}
void write_performance_file(int count)
{
    fprintf(file2,"CPU utilization = %.0f%%\n",((float)(total_run/lastfinish))*100);
    fprintf(file2,"Avg WTA = %.2f\n",(float)(total_WTA/count));
    fprintf(file2,"Avg Waiting = %.2f\n",(float)(total_wait/count));
}