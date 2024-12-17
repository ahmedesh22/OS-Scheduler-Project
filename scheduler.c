#include "headers.h"
#include "queue.h"
#include "Priqueue.h"

int msg_qid;
void Shortest_Job_First(struct priqueue* pq);
int finished_processes = 0;
void sighandler(int signum);
void Round_Robin_Scheduling(queue* Processes_Queue);
int Highest_Priority_First(struct priqueue* pq);
Scheduling_Algorithm scheduling_algorithm;
int quantum;
int count;
int noofprocess;




int main(int argc, char *argv[])
{
    initClk();
    signal(SIGUSR1, sighandler);
    struct msgbuff message;
    key_t msg_id=ftok("keyfile",'A');
    msg_qid=msgget(msg_id,0666|IPC_CREAT);
    scheduling_algorithm=atoi(argv[1]);
    quantum=atoi(argv[2]);
    count=atoi(argv[3]);
    noofprocess=count;
    file = fopen("scheduler.log", "w");
    if (file == NULL)
    {
        perror("Error, Cannot Open File\n");
        return -3;
    }
    fprintf(file, "#At time x process y state arr w total z remain y wait k\n");
    file2 = fopen("scheduler.perf", "w");
    if (file2 == NULL)
    {
        perror("Error, Cannot Open File\n");
        return -3;
    }
    struct queue* Processes_Queue = (struct queue* ) malloc(sizeof(struct queue));
    Processes_Queue->actualcount = 0;
    Processes_Queue->head = NULL;
    struct priqueue* Processes_PriQueue = (struct priqueue*) malloc(sizeof(struct priqueue));
    Processes_PriQueue->actualcount = 0;
    Processes_PriQueue->head = NULL;
    if (scheduling_algorithm == RR && quantum == -1) {
    perror("Missing quantum.\n");
    exit(1);
    }
    
    while (count > 0) 
    {
        while (true)
        {
            // perror("Error in receiving the message\n");
            // exit(-1);
            int receive_value = msgrcv (msg_qid, &message, sizeof(message.process), 0, IPC_NOWAIT);
            if (receive_value != -1 && (scheduling_algorithm == SJF))
            {
                struct prinode* Node = (struct prinode*) malloc(sizeof(struct prinode));
                setprinode(message.process, message.process.runningtime, READY,Node);
                prienqueue(Processes_PriQueue, Node);
                printf("enqueued id: %d\n", Node->process.id);
            }
            else if (receive_value != -1 && (scheduling_algorithm == HPF))
            {
                struct prinode* Node = (struct prinode*) malloc(sizeof(struct prinode));
                setprinode(message.process, message.process.priority, READY,Node);
                prienqueue(Processes_PriQueue, Node);
                //printf("enqueued id: %d\n", Node->process.id);
                count--;
            }
            else if (receive_value != -1 && scheduling_algorithm == RR)
            {
                struct node* new_node = (struct node*)malloc(sizeof(struct node));
                setnode(message.process, new_node);
                if(Processes_Queue->actualcount > 0){
                struct node* last_node = (struct node*)malloc(sizeof(struct node));
                struct PCB* current_process = NULL;
                current_process = (struct PCB*)malloc(sizeof(struct PCB));
                dequeue_from_back(Processes_Queue, current_process);
                setnode(*current_process, last_node);
                if(current_process->stoptime == getClk()){
                enqueue(new_node, Processes_Queue);
                enqueue(last_node, Processes_Queue);
                printf("Process %d preempted (remaining time: %d).\n",
                current_process->id, current_process->remainingTime);
                write_output_file(current_process, 2);
                }else{
                enqueue(last_node, Processes_Queue);
                enqueue(new_node, Processes_Queue);}
                }else{
                enqueue(new_node, Processes_Queue);
                }
                printf("Process %d enqueued (arrival: %d, runtime: %d)\n",
                message.process.id, message.process.arrivaltime, message.process.runningtime);
            }
            else
            {
                
                break;
            }
        }

        // After receiving put the process sent in the queue/priqueue.
        
        switch (scheduling_algorithm){
            case SJF: if (Processes_PriQueue->actualcount > 0)
            {
                Shortest_Job_First(Processes_PriQueue);
                pridequeue(&Processes_PriQueue->head->process, Processes_PriQueue);
                //printf("Process %d with the following arrival time received successfully: %d \n",message.process.id,message.process.arrivaltime);
                count--;
            }
            break;
            case RR: if (1)
            {
                
                Round_Robin_Scheduling(Processes_Queue);
            }   
            break;  
            case HPF: if (1)
            {
                //printf("this is the count of the priqueue %d \n",count);
                Highest_Priority_First(Processes_PriQueue);
            }
            break;
            default: break;
        }   
        //add rest of cases here

        
    }
    count=atoi(argv[3]);
    while((Processes_PriQueue->actualcount)>=0 && scheduling_algorithm==HPF)
    {
        if(Highest_Priority_First(Processes_PriQueue)==1)
        {
            break;
        }
    }
    while((Processes_Queue->actualcount)!=0 && scheduling_algorithm==RR)
    {
        Round_Robin_Scheduling(Processes_Queue);
    }
    // while(finished_processes != atoi(argv[3]))
    // {

    // }
    write_performance_file(atoi(argv[3]));
    free(Processes_Queue);
    free(Processes_PriQueue);
    fclose(file2);
    fclose(file);
    destroyClk(false);
    // kill(getppid(),SIGINT);
}

void Shortest_Job_First(struct priqueue* pq)
{
    // Look up the piqueue's head
    // Dequeue
    // fork
    // child execute process file
    
    struct PCB* current_process;
    current_process = &pq->head->process;
    int statloc;
    int ppid = getpid();
    //pridequeue(pd, pq);

    // if (index != 0)
    // {
    //     waitpid(processes_ids[index - 1], &statloc, 0);
    // }
    current_process->waittime = getClk() - current_process->arrivaltime;
    current_process->pid = fork();
    // waitpid(processes_ids[index], &statloc, 0);
    if (current_process->pid == -1)
    {
        perror("Error While Forking.\n");
        exit(-10);
    }
    else if (current_process->pid == 0 && getppid() == ppid) // Child
    {
        char id[10];
        sprintf(id, "%d", current_process->id);
        char arr[10];
        sprintf(arr, "%d", current_process->arrivaltime);
        char run[10];
        sprintf(run, "%d", current_process->runningtime);
        char pri[10];
        sprintf(pri, "%d", current_process->priority);
        char *arg[] = {"./process.out", id, arr, run, pri, NULL};
        if(execv("./process.out", arg) == -1)
        {
            perror("Error in Execv\n");
            exit(-10);
        }
    }
    else
    {
        //printf("Process with id %d started at time %d\n", current_process->id, getClk());
        current_process->starttime = getClk();
        write_output_file(current_process, 0);
        waitpid(current_process->pid, &statloc, 0);
        current_process->remainingTime = current_process->runningtime - (getClk() - current_process->starttime);
        write_output_file(current_process, 1);
        kill(current_process->pid,SIGKILL);
        //printf("Process with id %d finished at time %d\n", current_process->id, getClk());
    }
    finished_processes++;
}

void Round_Robin_Scheduling(queue* Processes_Queue)
{
    static struct PCB* current_process = NULL;
    static int last_time = -1;
    int current_time = getClk();
 
    // Check if a new process needs to be scheduled
    if (!current_process && Processes_Queue->actualcount > 0) {
        current_process = (struct PCB*)malloc(sizeof(struct PCB));
        dequeue(Processes_Queue, current_process);
        last_time = current_time;

        current_process->starttime = (current_process->starttime == -1) ? current_time : current_process->starttime;
         if(current_process->state == WAITING)
            {
                current_process->state = RUNNING;
                if(current_process->stoptime != getClk()){
                    printf("Process %d continued at time %d and its remaining time is %d.\n", current_process->id, getClk(), current_process->remainingTime);
                    write_output_file(current_process, 3);
                }
                else{
                    printf("Process %d still have the cpu for another quantum at time %d and its remaining time is %d.\n", current_process->id, getClk(), current_process->remainingTime);
                    write_output_file(current_process, 4);
                }
                current_process->waittime += getClk() - current_process->stoptime;
                
                kill(current_process->pid, SIGCONT);
            }
            else
            {
                printf("Process %d Started at time %d and its run time is %d.\n", current_process->id, getClk(),current_process->runningtime);
                current_process->waittime = getClk() - current_process->arrivaltime;
                write_output_file(current_process, 0);
                current_process->state = RUNNING;
                current_process->pid = fork();
                if (current_process->pid == 0) {
                    // Prepare arguments for execv
                    char id[10], arr[10], rem[10],quant[10],scAlgo[10];
                    sprintf(id, "%d", current_process->id);
                    sprintf(arr, "%d", current_process->arrivaltime);
                    //sprintf(rem, "%d", current_process->remainingTime >= quantum ? quantum : current_process->remainingTime);
                    sprintf(rem, "%d", current_process->remainingTime);
                    sprintf(quant, "%d", quantum);
                    sprintf(scAlgo, "%d", scheduling_algorithm);

                    char* args[] = {"./process.out", id, arr, rem, quant ,scAlgo, NULL};
                    last_time = current_time;
                    execv("./process.out", args);
                    perror("Exec failed");
                    exit(1);
                }
            }
        last_time = current_time;
    }

    // Handle the running process
    if (current_process) {
        // Check if the quantum has expired or process finished
        if ((current_time - last_time) >= quantum || (current_time - last_time) >= current_process->remainingTime  || current_process->remainingTime <= 0) {
            current_process->remainingTime -= (current_time - last_time);
            if (current_process->remainingTime <= 0) {
                // Process finished
                printf("Process %d finished execution. at time : %d\n", current_process->id, getClk());
                write_output_file(current_process, 1);
                finished_processes++;
                count--;
                kill(current_process->pid, SIGKILL);
                free(current_process);
                current_process = NULL;

            } else {
                if(Processes_Queue->actualcount == 0){
                current_time=getClk();
                last_time = current_time;
                current_process->id, current_process->remainingTime;
                current_process->stoptime = getClk();
                kill(current_process->pid, SIGCONT);
                }
             
            else {
                printf("Process %d preempted at time %d (remaining time: %d).\n",
                current_process->id, getClk() , current_process->remainingTime);
                current_process->stoptime = getClk();
                write_output_file(current_process, 2);
                kill(current_process->pid, SIGSTOP);
                }
                // Re-enqueue the process
                struct node* new_node = (struct node*)malloc(sizeof(struct node));
                setnode(*current_process, new_node);
                new_node->item.state = WAITING;
                enqueue(new_node, Processes_Queue);

                free(current_process);
                current_process = NULL;
            }
            last_time = current_time; // Update last_time for the next process
        }
    current_time=getClk();
    }
}



int Highest_Priority_First(struct priqueue* pq)
{
    static struct PCB* current_process = NULL;
    static int last_time = -1;
    int current_time = getClk();
    if (!current_process && pq->actualcount > 0)
    {
        current_process = (struct PCB*) malloc(sizeof(struct PCB));
        pridequeue(current_process, pq);
        current_process->state = RUNNING;
        current_process->starttime = current_time;
        printf("Starting process %d (runtime: %d, start time: %d)\n",
        current_process->id, current_process->runningtime, current_process->starttime);
        current_process->waittime=getClk()-current_process->arrivaltime;
        write_output_file(current_process,0);
        current_process->pid = fork();
        if (current_process->pid == -1)
        {
            perror("Error While Forking.\n");
            exit(-4);
        }
        else if (current_process->pid == 0) //Child Process
        {
            char id[10];
            sprintf(id, "%d", current_process->id);
            char arr[10];
            sprintf(arr, "%d", current_process->arrivaltime);
            char run[10];
            sprintf(run, "%d", current_process->remainingTime);
            char pri[10];
            sprintf(pri, "%d", current_process->priority);
            char *arg[] = {"./process.out",id, arr, run, pri, NULL};

            execv("./process.out", arg);
            perror("Error in execv.\n");
            exit(-3);
        }
        //last_time = current_time; // This is the last time a process started
        
    }

    else if (current_process && pq->actualcount > 0)
    {
        // Check the current_process with the head of the queue to see if the new process has a higher priority
        current_process->remainingTime -= (current_time - last_time);
        if (current_process->remainingTime <= 0)
        {
            printf("Process %d Finished at time %d.\n", current_process->id, getClk() );
            finished_processes++;
            write_output_file(current_process,1);
            kill(current_process->pid,SIGKILL);
            free(current_process);
            pridequeue(current_process, pq);            
            if(current_process->state == WAITING)
            {
                current_process->state = RUNNING;
                printf("Process %d continued at time %d with remaining time %d\n", current_process->id, getClk(),current_process->remainingTime);
                current_process->waittime += getClk()-current_process->stoptime;
                write_output_file(current_process,3);
                kill(current_process->pid, SIGCONT);
            }
            else
            {
                printf("Process %d Started at time %d.\n", current_process->id, getClk());
                current_process->waittime=getClk()-current_process->arrivaltime;
                write_output_file(current_process,0);          
                current_process->pid = fork();
                if (current_process->pid == -1)
                {
                    perror("Error While Forking.\n");
                    exit(-4);
                }
                else if (current_process->pid == 0) //Child Process
                {
                    char id[10];
                    sprintf(id, "%d", current_process->id);
                    char arr[10];
                    sprintf(arr, "%d", current_process->arrivaltime);
                    char run[10];
                    sprintf(run, "%d", current_process->remainingTime);
                    char pri[10];
                    sprintf(pri, "%d", current_process->priority);
                    char *arg[] = {"./process.out",id, arr, run, pri, NULL};
                    execv("./process.out", arg);
                    perror("Error in execv.\n");
                    exit(-3);
                }
                //last_time = current_time; // This is the last time a process started   
            }
        }
        else if (pq->head->pri < current_process->priority)
        {
            // Need to preempt the current_process and start the new process
            //Stop the running process
            printf("Process %d preempted (remaining time: %d).\n",
            current_process->id, current_process->remainingTime);
            current_process->state = WAITING;
            current_process->stoptime=getClk();
            write_output_file(current_process,2);
            kill(current_process->pid, SIGSTOP);
            last_time=current_time;
            struct prinode* new_node = (struct prinode*)malloc(sizeof(struct prinode));
            setprinode(*current_process, current_process->priority, WAITING,new_node);
            prienqueue(pq, new_node);
            free(current_process);
            pridequeue(current_process, pq);
            if(current_process->state == WAITING)
            {
                current_process->state = RUNNING;
                printf("Process %d continued at time %d.\n", current_process->id, getClk());
                current_process->waittime += getClk()-current_process->stoptime;
                write_output_file(current_process,3);
                kill(current_process->pid, SIGCONT);
            }
            else
            {
                printf("Process %d Started at time %d.\n", current_process->id, getClk());
                current_process->waittime=getClk()-current_process->arrivaltime;
                write_output_file(current_process,0);       
                current_process->pid = fork();
                if (current_process->pid == -1)
                {
                    perror("Error While Forking.\n");
                    exit(-4);
                }
                else if (current_process->pid == 0) //Child Process
                {
                    char id[10];
                    sprintf(id, "%d", current_process->id);
                    char arr[10];
                    sprintf(arr, "%d", current_process->arrivaltime);
                    char run[10];
                    sprintf(run, "%d", current_process->remainingTime);
                    char pri[10];
                    sprintf(pri, "%d", current_process->priority);
                    char *arg[] = {"./process.out",id, arr, run, pri, NULL};
                    execv("./process.out", arg);
                    perror("Error in execv.\n");
                    exit(-3);
                }
            }
            // Fork Or SIGCONT
            last_time = current_time;
        }
        // Else do nothing since the running process is of already higher priority
    }
    else if(pq->actualcount ==0 && noofprocess-finished_processes==1&&current_process)
    {
        current_process->remainingTime -= (current_time - last_time);
        if(current_process->remainingTime<=0)
        {
            printf("Process %d Finished at time %d.\n", current_process->id, getClk()); 
            write_output_file(current_process,1);
            kill(current_process->pid,SIGKILL);
            finished_processes++;  
            return 1;
        }
    }
    else if(current_process &&pq->actualcount==0&& noofprocess-finished_processes!=1)
    {
        current_process->remainingTime -= (current_time - last_time);
        if(current_process->remainingTime<=0)
        {
            printf("Process %d Finished at time %d.\n", current_process->id, getClk() ); 
            write_output_file(current_process,1);
            kill(current_process->pid,SIGKILL);
            finished_processes++;  
            current_process=NULL;
            return 0;
        }   
    }
    last_time=current_time;
    return 0;
    // Parent
    // The parent should constantly look up for any changes in the priqueue 
    // (if the head of the priqueue has a higher priority than the running process) 
    // then we should preempt (SIGSTOP) the running process and fork this new process
    // Then after this process finishes we should check whether we will SIGCONT the stopped
    // process or fork another process
}


void sighandler(int signum)
{
    //finished_processes++;
    //finished_processes++;
    signal(SIGUSR1, sighandler);
}