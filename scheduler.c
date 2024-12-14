#include "headers.h"
#include "queue.h"
#include "Priqueue.h"

int msg_qid;
int *processes_ids;
void SJF(struct priqueue* pq, int index);
int finished_processes = 0;
void sighandler(int signum);

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGUSR1, sighandler);
    
    int scheduling_algorithm=atoi(argv[1]);
    int quantum=atoi(argv[2]);
    int count=atoi(argv[3]);
    struct msgbuff message;
    key_t msg_id=ftok("keyfile",'A');
    msg_qid=msgget(msg_id,0666|IPC_CREAT);

    struct queue* Processes_Queue = (struct queue* ) malloc(sizeof(struct queue));
    Processes_Queue->actualcount = 0;
    Processes_Queue->head = NULL;
    struct priqueue* Processes_PriQueue = (struct priqueue*) malloc(sizeof(struct priqueue));
    Processes_PriQueue->actualcount = 0;
    Processes_PriQueue->head = NULL;

    processes_ids = (int *) malloc(sizeof(int) * count); // array of pids

    //Add a queue and a priqueue to store processes according to the scheduling algorithm sent from the process generator
    
    
    int i = 0;
    while (count > 0) 
    {
        while (true)
        {
            // perror("Error in receiving the message\n");
            // exit(-1);
            int receive_value = msgrcv (msg_qid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            if (receive_value != -1)
            {
                struct prinode* Node = (struct prinode*) malloc(sizeof(struct prinode));
                setprinode(message.process, message.process.runningtime, Node);
                prienqueue(Processes_PriQueue, Node);
                printf("enqueued id: %d\n", Processes_PriQueue->head->process.id);

            }
            else
            {
                
                break;
            }
        }

        // After receiving put the process sent in the queue/priqueue.
        if (scheduling_algorithm == 1) //SJF
        {
            // struct prinode* Node = (struct prinode*) malloc(sizeof(struct prinode));
            // setprinode(message.process, message.process.runningtime, Node);
            // prienqueue(Processes_PriQueue, Node);
            if (Processes_PriQueue->actualcount > 0)
            {
                SJF(Processes_PriQueue, i);
                pridequeue(&Processes_PriQueue->head->process, Processes_PriQueue);
                printf("Process %d with the following arrival time received successfully: %d \n",message.process.id,message.process.arrivaltime);
                i++;
                count--;
            }
            
            
        }
        //add rest of cases here

        
    }
    char msg[265]="I am done" ;

    while(finished_processes != atoi(argv[3])){

    }
    //int snd_id=msgsnd(msg_qid,&msg,sizeof(msg),!IPC_NOWAIT);

    free(Processes_Queue);
    free(Processes_PriQueue);
    free(processes_ids);
    destroyClk(true);
}

void SJF(struct priqueue* pq, int index)
{
    // Look up the piqueue's head
    // Dequeue
    // fork
    // child execute process file
    
    struct processData* pd;
    pd = &pq->head->process;
    int statloc;
    int ppid = getpid();
    //pridequeue(pd, pq);

    // if (index != 0)
    // {
    //     waitpid(processes_ids[index - 1], &statloc, 0);
    // }
    processes_ids[index] = fork();
    waitpid(processes_ids[index], &statloc, 0);
    if (processes_ids[index] == -1)
    {
        perror("Error While Forking.\n");
    }
    else if (processes_ids[index] == 0 && getppid() == ppid) // Child
    {
        char id[10];
        sprintf(id, "%d", pd->id);
        char arr[10];
        sprintf(arr, "%d", pd->arrivaltime);
        char run[10];
        sprintf(run, "%d", pd->runningtime);
        char pri[10];
        sprintf(pri, "%d", pd->priority);
        char *arg[] = {"./process.out", id, arr, run, pri, NULL};
        if(execv("./process.out", arg) == -1)
        {
            perror("DAH SOOOOT\n");
            exit(-10);
        }
    }
}

void sighandler(int signum)
{
    finished_processes++;
    signal(SIGUSR1, sighandler);
}