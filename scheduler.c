#include "headers.h"
#include "queue.h"
#include "Priqueue.h"
#include "Tree.h"
#include "Free_Memory_Table.h"
#include "Memory_Table.h"

int msg_qid;
void Shortest_Job_First(struct priqueue* pq);
int finished_processes = 0;
void Round_Robin_Scheduling(queue* Processes_Queue);
int Highest_Priority_First(struct priqueue* pq);
Scheduling_Algorithm scheduling_algorithm;
int quantum;
int count;
int noofprocess;

/* Memory Variables */
Memory_Table* MemTable;
Free_Memory_Table* FreeMemTable; // Don't Forget to free
BinaryTree* Tree;
struct priqueue* Waiting_Queue; //Queue for the processes that are waiting for memory

//Pri-Queue
struct priqueue* Processes_PriQueue;

int main(int argc, char *argv[])
{
    initClk();
    struct msgbuff message;
    key_t msg_id=ftok("keyfile",'A');
    msg_qid=msgget(msg_id,0666|IPC_CREAT);
    scheduling_algorithm=atoi(argv[1]);
    quantum=atoi(argv[2]);
    count=atoi(argv[3]);
    noofprocess=count;
    
    

    /* Memory Variables */
    MemTable = (Memory_Table*) malloc(sizeof(Memory_Table));
    FreeMemTable = (Free_Memory_Table*) malloc(sizeof(Free_Memory_Table));
    Tree = (BinaryTree*) malloc(sizeof(BinaryTree));
    Initialize_Tree(Tree);
    Free_Entry* f_entry = (Free_Entry*) malloc(sizeof(Free_Entry));
    Initialize_Free_Entry(f_entry, Tree->root);
    AddFreeEntry(FreeMemTable, f_entry);
    Waiting_Queue = (struct priqueue*) malloc(sizeof(struct priqueue));
    //printf("size: %d\n", FreeMemTable->head->node->size);


    // Now You have A Root (pointer to TreeNode, check Initialize_Tree) for the tree and I put it as the first free memory available to allocate into
    // When we receive a message, we should check the free memory table and find the smallest available block of memory that we can allocate into
    // --> Check Free_Memory_Table (I made the function AddFreeEntry to put free blocks of data as a linked list and this list is sorted from small to big)
    // So we can loop on the linked list and find the smallest place we can fit the process into
    // After that we can call AddTreeNodes if we need to split this block of data, or we can call AllocateTreeNode

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
    file3 = fopen("memory.log", "w");
    if (file3 == NULL)
    {
        perror("Error, Cannot Open File\n");
        return -3;
    }
    fprintf(file3, "#At time x allocated y bytes for process z from i to j\n");
    struct queue* Processes_Queue = (struct queue* ) malloc(sizeof(struct queue));
    Processes_Queue->actualcount = 0;
    Processes_Queue->head = NULL;
    Processes_PriQueue = (struct priqueue*) malloc(sizeof(struct priqueue));
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
                setprinode(message.process, message.process.runningtime, READY,Node); // I think here there is no need to send READY since its state is already READY
                // ask if we can allocate this process right now or not
                // Call CanAllocate()
                Free_Entry* free_e = CanAllocate(FreeMemTable, Node->process.memorysize);
                if (free_e == NULL)
                {
                    // Enqueue in the waiting queue
                    priwaitenqueue(Waiting_Queue, Node);
                }
                else
                {
                    // Now, the process can be allocated, check whether it needs splitting or not
                    if (CheckSize(Node->process.memorysize, free_e->size) == 1)
                    {
                        // We can Split
                        // Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
                        TreeNode* parent = RemoveFromFreeMemTable(FreeMemTable, free_e);
                        printf("After Free Memory Removal: Split\n");
                        //printFreeMemTable(FreeMemTable);
                        //printf("-------------\n");
                        TreeNode* ptr = Split(FreeMemTable, parent, Node->process.memorysize);

                        Entry* e = (Entry*) malloc(sizeof(Entry));
                        Initialize_Entry(e, Node->process.id, ptr);
                        AddEntry(MemTable, e);
                        printFreeMemTable(FreeMemTable);
                        printMemTable(MemTable);
                    }
                    else
                    {
                        // Here no need to split
                        free_e->node->status = 1; // Allocated
                        // Add this entry to Memory Table
                        Entry* e = (Entry*) malloc(sizeof(Entry));
                        Initialize_Entry(e, Node->process.id, free_e->node);
                        AddEntry(MemTable, e);
                        // Remove this entry from Free Memory Table
                        RemoveFromFreeMemTable(FreeMemTable, free_e);
                        printf("After Free Memory Removal: withu spliting\n");
                        printFreeMemTable(FreeMemTable);
                        printMemTable(MemTable);
                    }
                    write_memory_file(1, &Node->process, MemTable->head->Block->from, MemTable->head->Block->to);
                    prienqueue(Processes_PriQueue, Node);
                    printf("enqueued id: %d\n", Node->process.id);
                    
                }
                
                
            }
            else if (receive_value != -1 && (scheduling_algorithm == HPF))
            {
                struct prinode* Node = (struct prinode*) malloc(sizeof(struct prinode));
                setprinode(message.process, message.process.priority, READY,Node);
                Free_Entry* free_e = CanAllocate(FreeMemTable, Node->process.memorysize);
                if (free_e == NULL)
                {
                    // Enqueue in the waiting queue
                    priwaitenqueue(Waiting_Queue, Node);
                }
                else
                {
                    // Now, the process can be allocated, check whether it needs splitting or not
                    if (CheckSize(Node->process.memorysize, free_e->size) == 1)
                    {
                        // We can Split
                        // Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
                        TreeNode* parent = RemoveFromFreeMemTable(FreeMemTable, free_e);
                        printf("After Free Memory Removal: Split\n");
                        //printFreeMemTable(FreeMemTable);
                        //printf("-------------\n");
                        TreeNode* ptr = Split(FreeMemTable, parent, Node->process.memorysize);

                        Entry* e = (Entry*) malloc(sizeof(Entry));
                        Initialize_Entry(e, Node->process.id, ptr);
                        AddEntry(MemTable, e);
                        printFreeMemTable(FreeMemTable);
                        printMemTable(MemTable);
                    }
                    else
                    {
                        // Here no need to split
                        free_e->node->status = 1; // Allocated
                        // Add this entry to Memory Table
                        Entry* e = (Entry*) malloc(sizeof(Entry));
                        Initialize_Entry(e, Node->process.id, free_e->node);
                        AddEntry(MemTable, e);
                        // Remove this entry from Free Memory Table
                        RemoveFromFreeMemTable(FreeMemTable, free_e);
                        printf("After Free Memory Removal: withu spliting\n");
                        printFreeMemTable(FreeMemTable);
                        printMemTable(MemTable);
                    }
                    write_memory_file(1, &Node->process, MemTable->head->Block->from, MemTable->head->Block->to);
                    prienqueue(Processes_PriQueue, Node);
                    printf("enqueued id: %d\n", Node->process.id);
                }
                
                
            }
            else if (receive_value != -1 && scheduling_algorithm == RR)
            {
                struct node* new_node = (struct node*)malloc(sizeof(struct node));
                setnode(message.process, new_node);

                //Check if the proccess can be allocated or not 
                Free_Entry* free_e = CanAllocate(FreeMemTable, new_node->item.memorysize);
                if (free_e == NULL) // the process cannot be allocated
                {
                    struct prinode* temp_priNode = (struct prinode*) malloc(sizeof(struct prinode));
                    Node_to_PriNode(new_node, temp_priNode); 
                    // Enqueue in the waiting queue
                    priwaitenqueue(Waiting_Queue, temp_priNode);
                }
                else
                {
                    // Now, the process can be allocated, check whether it needs splitting or not
                    if (CheckSize(new_node->item.memorysize, free_e->size) == 1)
                    {
                        // We can Split
                        // Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
                        TreeNode* parent = RemoveFromFreeMemTable(FreeMemTable, free_e);
                        printf("After Free Memory Removal: Split\n");
                        //printFreeMemTable(FreeMemTable);
                        //printf("-------------\n");
                        TreeNode* ptr = Split(FreeMemTable, parent, new_node->item.memorysize);

                        Entry* e = (Entry*) malloc(sizeof(Entry));
                        Initialize_Entry(e, new_node->item.id, ptr);
                        AddEntry(MemTable, e);
                        printFreeMemTable(FreeMemTable);
                        printMemTable(MemTable);
                    }
                    else
                    {
                        // Here no need to split
                        free_e->node->status = 1; // Allocated
                        // Add this entry to Memory Table
                        Entry* e = (Entry*) malloc(sizeof(Entry));
                        Initialize_Entry(e, new_node->item.id, free_e->node);
                        AddEntry(MemTable, e);
                        // Remove this entry from Free Memory Table
                        RemoveFromFreeMemTable(FreeMemTable, free_e);
                        printf("After Free Memory Removal: withu spliting\n");
                        printFreeMemTable(FreeMemTable);
                        printMemTable(MemTable);
                    }
                    write_memory_file(1, &new_node->item, MemTable->head->Block->from, MemTable->head->Block->to);
                    printf("enqueued id: %d\n", new_node->item.id);
                    if(Processes_Queue->actualcount > 0)
                    {
                        struct node* last_node = (struct node*)malloc(sizeof(struct node));
                        struct PCB* current_process = NULL;
                        current_process = (struct PCB*)malloc(sizeof(struct PCB));
                        dequeue_from_back(Processes_Queue, current_process);
                        setnode(*current_process, last_node);
                        if(current_process->stoptime == getClk())
                        {
                            enqueue(new_node, Processes_Queue);
                            enqueue(last_node, Processes_Queue);
                            printf("Process %d preempted (remaining time: %d).\n",
                            current_process->id, current_process->remainingTime);
                            write_output_file(current_process, 2);
                        }
                        else
                        {
                            enqueue(last_node, Processes_Queue);
                            enqueue(new_node, Processes_Queue);
                        }
                    }
                    else
                    {
                        enqueue(new_node, Processes_Queue);
                    }
                    printf("Process %d enqueued (arrival: %d, runtime: %d)\n",
                    message.process.id, message.process.arrivaltime, message.process.runningtime);
                }
                
            }
            else
            {
                if((scheduling_algorithm == SJF)&&Waiting_Queue->head)
                {
                    Free_Entry* free_e = CanAllocate(FreeMemTable, Waiting_Queue->head->process.memorysize);
                    if (free_e)
                    {
                        struct PCB*process=(struct PCB*)malloc(sizeof(struct PCB));
                        pridequeue(process,Waiting_Queue);
                        printf("b3d el dequeue id %d \n",process->id);
                        struct prinode* Node = (struct prinode*) malloc(sizeof(struct prinode));
                        setprinode(*process, process->runningtime, READY,Node);
                        if (CheckSize(Node->process.memorysize, free_e->size) == 1)
                        {
                            // We can Split
                            // Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
                            TreeNode* parent = RemoveFromFreeMemTable(FreeMemTable, free_e);
                            printf("From WAiting After Free Memory Removal: Split\n");
                            //printFreeMemTable(FreeMemTable);
                            //printf("-------------\n");
                            TreeNode* ptr = Split(FreeMemTable, parent, Node->process.memorysize);

                            Entry* e = (Entry*) malloc(sizeof(Entry));
                            Initialize_Entry(e, Node->process.id, ptr);
                            AddEntry(MemTable, e);
                            printFreeMemTable(FreeMemTable);
                            printMemTable(MemTable);
                        }
                        else
                        {
                            // Here no need to split
                            free_e->node->status = 1; // Allocated
                            // Add this entry to Memory Table
                            Entry* e = (Entry*) malloc(sizeof(Entry));
                            Initialize_Entry(e,Node->process.id, free_e->node);
                            AddEntry(MemTable, e);
                            // Remove this entry from Free Memory Table
                            RemoveFromFreeMemTable(FreeMemTable, free_e);
                            printf("From WaitingAfter Free Memory Removal: withu spliting\n");
                            printFreeMemTable(FreeMemTable);
                            printMemTable(MemTable);
                        }
                        prienqueue(Processes_PriQueue, Node);
                        printf("enqueued From Waiting id: %d\n", Node->process.id);
                        free(process);
                    }
                }
                if((scheduling_algorithm == HPF)&&Waiting_Queue->head)
                {
                    Free_Entry* free_e = CanAllocate(FreeMemTable, Waiting_Queue->head->process.memorysize);
                    if (free_e)
                    {
                        struct PCB*process=(struct PCB*)malloc(sizeof(struct PCB));
                        pridequeue(process,Waiting_Queue);
                        printf("b3d el dequeue id %d \n",process->id);
                        struct prinode* Node = (struct prinode*) malloc(sizeof(struct prinode));
                        setprinode(*process, process->priority, READY,Node);
                        if (CheckSize(Node->process.memorysize, free_e->size) == 1)
                        {
                            // We can Split
                            // Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
                            TreeNode* parent = RemoveFromFreeMemTable(FreeMemTable, free_e);
                            printf("From WAiting After Free Memory Removal: Split\n");
                            //printFreeMemTable(FreeMemTable);
                            //printf("-------------\n");
                            TreeNode* ptr = Split(FreeMemTable, parent, Node->process.memorysize);

                            Entry* e = (Entry*) malloc(sizeof(Entry));
                            Initialize_Entry(e, Node->process.id, ptr);
                            AddEntry(MemTable, e);
                            printFreeMemTable(FreeMemTable);
                            printMemTable(MemTable);
                        }
                        else
                        {
                            // Here no need to split
                            free_e->node->status = 1; // Allocated
                            // Add this entry to Memory Table
                            Entry* e = (Entry*) malloc(sizeof(Entry));
                            Initialize_Entry(e,Node->process.id, free_e->node);
                            AddEntry(MemTable, e);
                            // Remove this entry from Free Memory Table
                            RemoveFromFreeMemTable(FreeMemTable, free_e);
                            printf("From WaitingAfter Free Memory Removal: withu spliting\n");
                            printFreeMemTable(FreeMemTable);
                            printMemTable(MemTable);
                        }
                        prienqueue(Processes_PriQueue, Node);
                        printf("enqueued From Waiting id: %d\n", Node->process.id);
                        free(process);
                    }
                }
                
                

                if((scheduling_algorithm == RR)&&Waiting_Queue->head)
                {   
                    
                    Free_Entry* free_e = CanAllocate(FreeMemTable, Waiting_Queue->head->process.memorysize);
                    if (free_e) // if the smallest memory size process waiting can be allocated
                    {   printf("wait can be allocated\n");
                        struct PCB* process=(struct PCB*)malloc(sizeof(struct PCB));
                        pridequeue(process,Waiting_Queue);
                        printf("b3d el dequeue id %d \n",process->id);
                        struct node* new_node = (struct node*)malloc(sizeof(struct node));
                        setnode(*process, new_node);
                        if (CheckSize(new_node->item.memorysize, free_e->size) == 1)
                        {
                            // We can Split
                            // Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
                            TreeNode* parent = RemoveFromFreeMemTable(FreeMemTable, free_e);
                            printf("From WAiting After Free Memory Removal: Split\n");
                            //printFreeMemTable(FreeMemTable);
                            //printf("-------------\n");
                            TreeNode* ptr = Split(FreeMemTable, parent, new_node->item.memorysize);

                            Entry* e = (Entry*) malloc(sizeof(Entry));
                            Initialize_Entry(e, new_node->item.id, ptr);
                            AddEntry(MemTable, e);
                            printFreeMemTable(FreeMemTable);
                            printMemTable(MemTable);
                        }
                        else
                        {
                            // Here no need to split
                            free_e->node->status = 1; // Allocated
                            // Add this entry to Memory Table
                            Entry* e = (Entry*) malloc(sizeof(Entry));
                            Initialize_Entry(e,new_node->item.id, free_e->node);
                            AddEntry(MemTable, e);
                            // Remove this entry from Free Memory Table
                            RemoveFromFreeMemTable(FreeMemTable, free_e);
                            printf("From WaitingAfter Free Memory Removal: withu spliting\n");
                            printFreeMemTable(FreeMemTable);
                            printMemTable(MemTable);
                        }
                        enqueue(new_node, Processes_Queue);
                        printf("enqueued From Waiting id: %d\n", new_node->item.id);
                        free(process);
                    }
                }
                break;
            }
        }

        // After receiving put the process sent in the queue/priqueue.
        
        switch (scheduling_algorithm){
            case SJF: 
                Shortest_Job_First(Processes_PriQueue);
                break;


            case RR: 
                Round_Robin_Scheduling(Processes_Queue);
                break;  


            case HPF: 
                Highest_Priority_First(Processes_PriQueue);
                break;

            default: break;
        }   
        
    }
    count=atoi(argv[3]);

    while((Processes_Queue->actualcount)!=0 && scheduling_algorithm==RR)
    {
        Round_Robin_Scheduling(Processes_Queue);
    }

    write_performance_file(atoi(argv[3]));
    free(Processes_Queue);
    free(Processes_PriQueue);

    FreeMT(MemTable); // to free entries in the memory table
    free(MemTable); // free memory table itself
    FreeFMT(FreeMemTable); 
    free(FreeMemTable);
    FreeTree(Tree->root); // free all tree nodes
    free(Tree); // free tree itself
    fclose(file3);
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
    static struct PCB* current_process = NULL;
    
    
    int statloc;
    int schd_pid = getpid();
    //pridequeue(pd, pq);


    if (!current_process) // There is no current process on the CPU, Then Fork a process and start it
    {
        if (!priisempty(pq))
        {
            current_process = (struct PCB*) malloc(sizeof(struct PCB));
            // struct PCB temp_process;
            // if (pridequeue(&temp_process, Processes_PriQueue))
            // {
            //     *current_process = temp_process;
            // }
            pridequeue(current_process, Processes_PriQueue);
        }
        else
        {
            return;
        }
        /* Starting The Process */
        current_process->waittime = getClk() - current_process->arrivaltime;
        current_process->starttime = getClk();
        current_process->pid = fork();
        // waitpid(processes_ids[index], &statloc, 0);
        if (current_process->pid == -1)
        {
            perror("Error While Forking.\n");
            exit(-10);
        }
        else if (current_process->pid == 0 && getppid() == schd_pid) // Child
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
            
            printf("Process with id %d started at time %d\n", current_process->id, getClk());
            current_process->starttime = getClk();
            write_output_file(current_process, 0);
            return;
        }
    }
    else // There is a current process in the CPU
    {
        // Check if the process has finished
        if (current_process->runningtime == getClk() - current_process->starttime) // Process is finished
        {
            current_process->remainingTime = current_process->runningtime - (getClk() - current_process->starttime);
            write_output_file(current_process, 1);
            printf("Process with id %d finished at time %d\n", current_process->id, getClk());
            Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
            TreeNode* nodeptr = RemoveFromMemTable(MemTable, current_process->id);
            write_memory_file(0, current_process, nodeptr->from, nodeptr->to);
            nodeptr=Merge(FreeMemTable,nodeptr);
            Initialize_Free_Entry(fe, nodeptr);
            AddFreeEntry(FreeMemTable, fe);
            printf("------------- A process has Finished ------- \n");
            printFreeMemTable(FreeMemTable);
            printMemTable(MemTable);
            free(current_process);
            kill(current_process->pid,SIGKILL);
            finished_processes++;
            count--;
            current_process = NULL;
        }
        else // Not Finished Yet
        {
            return;
        }
    }
    
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

                Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
                TreeNode* nodeptr = RemoveFromMemTable(MemTable, current_process->id);
                write_memory_file(0, current_process, nodeptr->from, nodeptr->to);
                nodeptr=Merge(FreeMemTable,nodeptr);
                Initialize_Free_Entry(fe, nodeptr);
                AddFreeEntry(FreeMemTable, fe);
                printf("------------- A process has Finished ------- \n");
                
                printFreeMemTable(FreeMemTable);
                printMemTable(MemTable);

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
    if(!current_process &&  pq->actualcount>0)
    {
        current_process = (struct PCB*)malloc(sizeof(struct PCB));
        pridequeue(current_process,pq);
        if(current_process->state==WAITING)
        {
            current_process->state = RUNNING;
            current_process->waittime += getClk()-current_process->stoptime;
            printf("Process %d continued at time %d with remaining time %d\n", current_process->id, getClk(),current_process->remainingTime);
            write_output_file(current_process,3);
            kill(current_process->pid, SIGCONT);
        }
        else if(current_process->state==READY)
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
    }
    else if(current_process)
    {
        current_process->remainingTime -= (current_time-last_time);
        if(current_process->remainingTime<=0)
        {
            printf("Process %d finished execution. at time : %d\n", current_process->id, getClk());
            write_output_file(current_process, 1);
            Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
            TreeNode* nodeptr = RemoveFromMemTable(MemTable, current_process->id);
            write_memory_file(0, current_process, nodeptr->from, nodeptr->to);
            nodeptr=Merge(FreeMemTable,nodeptr);
            Initialize_Free_Entry(fe, nodeptr);
            AddFreeEntry(FreeMemTable, fe);
            printf("------------- A process has Finished ------- \n");
            
            printFreeMemTable(FreeMemTable);
            printMemTable(MemTable);
            finished_processes++;
            count--;
            kill(current_process->pid, SIGKILL);
            free(current_process);
            current_process = NULL;

        }
        else if(pq->head&&current_process->priority>pq->head->process.priority)
        {
            printf("Process %d preempted at time %d (remaining time: %d).\n",
            current_process->id, getClk() , current_process->remainingTime);
            current_process->stoptime = getClk();
            write_output_file(current_process, 2);
            current_process->state=WAITING;
            kill(current_process->pid, SIGSTOP);
            struct prinode* node=(struct prinode*)malloc(sizeof(struct prinode));
            setprinode(*current_process,current_process->priority,current_process->state,node);
            prienqueue(pq,node);
            free(current_process);
            current_process=NULL;
        }
    }
    last_time=current_time;
}
