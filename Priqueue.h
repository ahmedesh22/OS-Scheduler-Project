#pragma once
#include "headers.h"

struct prinode;
struct prinode
{
    struct processData process;
    int pri;
    struct prinode * next;

};
void setprinode(struct processData item,int priority,P_state state,struct prinode *node)
{
    if(node==NULL)
    {
        return;
    }
    node->process=item;
    node->pri=priority;
    node->next =NULL;
    node->process.state=state;
    
}
struct priqueue
{
    struct prinode * head;
    int actualcount;
};
void setpriqueue(struct priqueue *queue)
{
    if(queue==NULL)
    {
        return;
    }
    queue->head =NULL;
    queue->actualcount=0;
}

void printpriqueue(struct priqueue* q)
{
    struct prinode* pq = q->head;
    // pq->pri=q->head->pri;
    while(pq)
    {
        printf("pri: %d runtime: %d arrtime: %d\n", pq->pri, pq->process.runningtime, pq->process.arrivaltime);
        pq = pq->next;
    }
}
bool priisempty(struct priqueue *queue)
{
    if(queue->actualcount == 0)
    {
        return true;
    }
    return false;
}
void prienqueue( struct priqueue *queue,struct prinode *node)
{
    if(queue->head == NULL || queue->head->pri > node->pri)
    {
        node->next=queue->head;
        queue->head = node;
        queue->head->process = node->process;
        queue->head->pri = node->pri;
        
        queue->actualcount++;
        return;
    }
    struct prinode *current = queue->head;
    while(current->next !=NULL && (current->next->pri)<=(node->pri))
    {
        current=current->next;
    }
    node->next=current->next;
    current->next=node;
    queue->actualcount++;
}
bool pridequeue(struct processData *pd,struct priqueue *queue)
{
    if(priisempty(queue))
    {
        return false;
    }
    *pd=queue->head->process;
    struct prinode * deleteptr =queue->head;
    queue->head=queue->head->next;
    free(deleteptr);
    queue->actualcount--;
    return true;
}