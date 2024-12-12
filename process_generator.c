#include "headers.h"
#include "queue.h"
void clearResources(int);
int msg_qid;
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    struct queue* Processes_Queue = (struct queue*)malloc(sizeof(struct queue));
    Processes_Queue->head = NULL;
    Processes_Queue->actualcount = 0;
    int scheduling_algorithm;
    int quantum = -1;
    char *arg[] = {"./clk", NULL};
    char *arg2[] = {"./scheduler", argv[3], "-1", NULL};
    key_t msg_id=ftok("keyfile",2);
    msg_id=msgget(msg_id,0666|IPC_CREAT);
    if (argc < 4)
    {
        printf("Not enough information provided, cannot simulate.\n");
        exit(-1); 
    }
    else
    {
        // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
        //make sure that the scheduling algo is valid and it has its paramaters if needed
        if (strcmp(argv[2],"-sch") != 0)
        {
            printf("Please enter information in this scheme for example /process_generator.o testcase.txt -sch 4 -q 2\n");
            exit(-1);
        }
        
        if (strcmp(argv[3],"1") != 0 && strcmp(argv[3],"2") != 0 && strcmp(argv[3],"3") != 0 && strcmp(argv[3],"4") != 0)
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

        // printf("Chosen algo %d\n", scheduling_algorithm);
        // printf("Quanta %d\n", quantum);
        
        //Here, we need a queue to read the processes from the input file and put it in the queue.
        FILE *file;

        file = fopen("processes.txt", "r");
        if (file == NULL)
        {
            printf("Could not Open file for reading\n");
            exit(-2);
        }       

        //skip the first line
        char line[256];
        fgets(line, sizeof(line), file);
        
        //Read the real data
        int id, arrival_time, run_time, priority;
        

        
        while (fscanf(file, "%d\t%d\t%d\t%d\n", &id, &arrival_time, &run_time, &priority) != EOF)
        {
            struct processData* process = (struct processData*)malloc(sizeof(struct processData));
            init_process(id, arrival_time, run_time, priority, process);
            struct node *Node = (struct node *)malloc(sizeof(struct node)); 
            Node->item = *process; 
            Node->next = NULL;                
            enqueue(Node, Processes_Queue);
            //printf("Read Numbers: %d, %d, %d, %d\n", id, arrival_time, run_time , priority);
        }
        print_queue(Processes_Queue);
        
        fclose(file); 
        //Now we have the Processes and the correct scheduling algorithm with its corresponding parameters if needed
    }

    
    
   
    
    
    // 3. Initiate and create the scheduler and clock processes.

    //Fork The clock
    int pid_clk, pid_schd;
    pid_clk = fork();
    if (pid_clk == -1)
    {
        perror("Error while forking.");
    }
    
    else if (pid_clk == 0) // First Child Clock
    {
        execv("./clk", arg);
    }
    else
    {
        pid_schd = fork();
        if(pid_schd == -1)
        {
            perror("Error while forking.");
        }
        else if(pid_schd == 0)
        {
            if(execv("./scheduler", arg2)==-1)
            {
                perror("execv failed");
                return -1;  
            }
        }
        else
        {
            initClk();
            // To get time use this function. 
            int snd_id,rcv_id;
            int x = getClk();
            printf("Current Time is %d\n", x);
            // TODO Generation Main Loop
            struct msgbuff message;
            char msg[256];
            
            struct node* Current = Processes_Queue->head;
            struct node* Next;
            while (!isempty(Processes_Queue) )
            {
                if(Processes_Queue->head->item.arrivaltime == getClk())
                {
                    sprintf(msg,"%d\t%d\t%d\t%d\n",Processes_Queue->head->item.id,Processes_Queue->head->item.arrivaltime,Processes_Queue->head->item.runningtime,Processes_Queue->head->item.priority);
                    strcpy(message.mtext,msg);
                    snd_id=msgsnd(msg_qid,&message,sizeof(message),!IPC_NOWAIT);
                    if (snd_id==-1)
                    {
                        perror("COULDNT SEND ");
                        return 0;
                    }
                    Next = Current->next;
                    dequeue(Current,Processes_Queue);
                    free(Current);
                    Current = Next; 
                }
                   
            }
            rcv_id=msgrcv(msg_qid,&message,sizeof(message),0,!IPC_NOWAIT);
            
            // 5. Create a data structure for processes and provide it with its parameters.
            // 6. Send the information to the scheduler at the appropriate time.
            // 7. Clear clock resources
            // put it in clear resources
        }
// 4. Use this function after creating the clock process to initialize clock.
    }
}

void clearResources(int signum)
{
    destroyClk(true); 
    killpg(getpgrp(),SIGKILL);
    kill(getpid(),SIGKILL);
    signal(SIGINT, clearResources);
    //TODO Clears all resources in case of interruption
}