#include "headers.h"
struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int starttime;
    int stoptime;
    int waittime;
    int id;
};

// function to initialize a process
void init_process(int id, int arr, int run, int pr, struct processData *process)
{
    process->id = id;
    process->arrivaltime = arr;
    process->runningtime = run;
    process->priority = pr;
}
struct node;
struct node
{
    struct processData item; 
    struct node *next; 
};
struct queue
{
    struct node *head; 
    int actualcount;
};
bool isempty(struct queue *queue)
{
    if(queue->actualcount ==0)
    {
        return true;
    }
    return false;
}
bool dequeue(struct node *node,struct queue *queue)
{
    if(isempty(queue)) 
    {
        return false;
    }
    node=queue->head;
    queue->head=queue->head->next;
    queue->actualcount--;
    return true;
}
void enqueue(struct node *node,struct queue *queue)
{
    if(queue->head==NULL)
    {
        queue->head=node;
        queue->actualcount++;
        return; 
    }
    struct node *current = queue->head;
    while(current->next !=NULL)
    {
        current=current->next;
    }
    current->next=node;
    queue->actualcount++;
}

int getActualCount(struct queue *q)
{
    printf("%d\n", q->actualcount);
}
// function to test the queue
void print_queue(struct queue *q)
{
    struct node *current = q->head;
    while (current)
    {
        printf("%d\n", current->item.arrivaltime);
        current = current->next;
    }
}

