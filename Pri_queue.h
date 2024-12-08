#include <cstddef>
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
struct prinode
{
    struct processData process;
    int pri;
    struct prinode * next=NULL;

};
void setprinode(struct processData item,int priority,struct prinode *node)
{
    
    node->process=item;
    node->pri=priority;
}
struct priqueue
{
    struct prinode * head=NULL;
    int actualcount = 0;
};
bool isempty(struct priqueue *queue)
{
    if(queue->actualcount ==0)
    {
        return true;
    }
    return false;
}
void enqueue( struct priqueue *queue,struct prinode *node)
{
    if(queue->head == NULL ||queue->head->pri < node->pri)
    {
        node->next=queue->head;
        queue->head = node;
        queue->actualcount++;
        return;
    }
    struct prinode *current = queue->head;
    while(current->next !=NULL && (current->next->pri)>=(node->pri))
    {
        current=current->next;
    }
    node->next=current->next;
    current->next=node;
    queue->actualcount++;
}
bool dequeue(struct prinode *node,struct priqueue *queue)
{
    if(isempty)
    {
        return false;
    }
    node=queue->head;
    queue->head=queue->head->next;
    queue->actualcount--;
    return true;
}