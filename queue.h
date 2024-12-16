#pragma once
#include "headers.h"


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
typedef struct queue
{
    struct node *head; 
    int actualcount;
} queue;

void setnode(struct processData item, struct node *node)
{
    if(node==NULL)
    {
        return;
    }
    node->item=item;
    node->next =NULL;
}

bool isempty(struct queue *queue)
{
    if(queue->actualcount ==0)
    {
        return true;
    }
    return false;
}
bool dequeue(struct queue*queue,struct processData* data)
{
    if(isempty(queue))
    {
        return false;
    }
    struct node * deleteptr =queue->head;
    *data=queue->head->item;
    queue->head = queue->head->next;
    free(deleteptr);
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

void getActualCount(struct queue *q)
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


bool dequeue_from_back(struct queue *queue, struct processData *data)
{
    if (isempty(queue))
    {
        return false;
    }

    // If there is only one node
    if (queue->head->next == NULL)
    {
        *data = queue->head->item;
        free(queue->head);
        queue->head = NULL;
        queue->actualcount--;
        return true;
    }

    // Traverse to the second last node
    struct node *current = queue->head;
    while (current->next->next != NULL)
    {
        current = current->next;
    }

    // Remove the last node
    struct node *deleteptr = current->next;
    *data = deleteptr->item;
    current->next = NULL;
    free(deleteptr);
    queue->actualcount--;
    return true;
}