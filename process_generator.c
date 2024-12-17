#include "headers.h"
#include "queue.h"
void clearResources(int);
int msg_qid;
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    struct queue *Processes_Queue = (struct queue *)malloc(sizeof(struct queue));
    Processes_Queue->head = NULL;
    Processes_Queue->actualcount = 0;
    int scheduling_algorithm;
    int quantum = -1;
    char *arg[] = {"./clk.out", NULL};
    char *arg2[] = {"./scheduler.out", argv[3], "-1", "00000", NULL};
    key_t msg_id = ftok("keyfile", 'A');
    msg_qid = msgget(msg_id, 0666 | IPC_CREAT);
    if (msg_qid == -1)
    {
        perror("Error in creating message queue.\n");
        exit(-3);
    }
    if (argc < 4)
    {
        printf("Not enough information provided, cannot simulate.\n");
        exit(-1);
    }
    else
    {
        // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
        // make sure that the scheduling algo is valid and it has its paramaters if needed
        if (strcmp(argv[2], "-sch") != 0)
        {
            printf("Please enter information in this scheme for example /process_generator.o testcase.txt -sch 4 -q 2\n");
            exit(-1);
        }

        if (strcmp(argv[3], "1") != 0 && strcmp(argv[3], "2") != 0 && strcmp(argv[3], "3") != 0 && strcmp(argv[3], "4") != 0)
        {
            printf("Invalid Scheduling algorithm\n");
            exit(-1);
        }
        scheduling_algorithm = atoi(argv[3]);
        if (scheduling_algorithm != 1 && scheduling_algorithm != 2) // if not SJF  and not HPF then look for the extra parameter Quantum
        {
            if (argc < 6)
            {
                printf("Needed Paramaters aren't provided, cannot simulate.\n");
                exit(-1);
            }
            if (strcmp(argv[4], "-q") != 0)
            {
                printf("Please enter information in this scheme for example /process_generator.o testcase.txt -sch 5 -q 2\n");
                exit(-1);
            }
            else
            {
                quantum = atoi(argv[5]);
                if (quantum < 1)
                {
                    printf("Quantum cannot be less than 0, try again\n");
                    exit(-1);
                }
                arg2[2] = argv[5];
            }
        }
        else
        {
            if (argc > 4)
            {
                printf("Additional paramaters are ignored\n");
            }
        }

        // printf("Chosen algo %d\n", scheduling_algorithm);
        // printf("Quanta %d\n", quantum);

        // Here, we need a queue to read the processes from the input file and put it in the queue.
        FILE *file;

        file = fopen("processes.txt", "r");
        if (file == NULL)
        {
            printf("Could not Open file for reading\n");
            exit(-2);
        }

        // skip the first line
        char line[256];
        fgets(line, sizeof(line), file);

        // Read the real data
        int id, arrival_time, run_time, priority;

        while (fscanf(file, "%d\t%d\t%d\t%d\n", &id, &arrival_time, &run_time, &priority) != EOF)
        {
            struct PCB *process = (struct PCB *)malloc(sizeof(struct PCB));
            init_process(id, arrival_time, run_time, priority, process);
            process->remainingTime = run_time;
            process->state = READY;
            struct node *Node = (struct node *)malloc(sizeof(struct node));
            Node->item = *process;
            Node->next = NULL;
            enqueue(Node, Processes_Queue);
            // printf("Read Numbers: %d, %d, %d, %d\n", id, arrival_time, run_time , priority);
        }

        // itoa(Processes_Queue->actualcount, arg2[3], 10);
        char temp[5];
        sprintf(temp, "%d", Processes_Queue->actualcount);
        arg2[3] = temp;
        // printf("%s      %s\n", arg2[3], arg2[2]);
        // print_queue(Processes_Queue);

        fclose(file);
        // Now we have the Processes and the correct scheduling algorithm with its corresponding parameters if needed
    }

    // 3. Initiate and create the scheduler and clock processes.

    // Fork The clock
    int pid_clk, pid_schd;
    pid_clk = fork();
    if (pid_clk == -1)
    {
        perror("Error while forking.");
        exit(-2);
    }

    else if (pid_clk == 0) // First Child Clock
    {
        // execv("./clk", arg);
        printf("I am the Clock. My pID is %d and my PPID is %d\n", getpid(), getppid());
        if (execv("./clk.out", arg) == -1)
        {
            perror("execv failed in clock");
            return -1;
        }
    }
    initClk();
    pid_schd = fork();
    if (pid_schd == -1)
    {
        perror("Error while forking.");
    }
    else if (pid_schd == 0)
    {
        printf("I am the Scheduler. My pID is %d and my PPID is %d\n", getpid(), getppid());
        if (execv("./scheduler.out", arg2) == -1)
        {
            perror("execv failed in scheduler");
            return -1;
        }
    }
    // To get time use this function.
    int snd_id, rcv_id;
    int x = getClk();
    printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    struct msgbuff message;
    while (!isempty(Processes_Queue))
    {
        
        if (Processes_Queue->head->item.arrivaltime == getClk())
        {
            //Dequeue(Processes_Queue, Current);
            //message.process=Current->item;
            dequeue(Processes_Queue,&message.process);
            //printf("id : %d arr: %d run: %d pri: %d \n",message.process.id,message.process.arrivaltime,message.process.runningtime,message.process.priority);
            message.mtype = 1;
            //printf("message   %d \n",msg_qid);
            if (msgsnd(msg_qid, &message, sizeof(message.process), IPC_NOWAIT) == -1)
            {
                perror("Error sending message");
            }
        }
    }
    // 4. Use this function after creating the clock process to initialize clock.
    //rcv_id=msgrcv(msg_qid,&message,sizeof(message),0,!IPC_NOWAIT);
    free(Processes_Queue);
    int statloc;
    waitpid(pid_schd, &statloc, 0);
    raise(SIGINT);
    
}

void clearResources(int signum)
{
    destroyClk(true);
    msgctl(msg_qid,IPC_RMID, (struct msqid_ds *)0);
    killpg(getpgrp(), SIGKILL);
    kill(getpid(), SIGKILL);
    signal(SIGINT, clearResources);
    
    // TODO Clears all resources in case of interruption
}