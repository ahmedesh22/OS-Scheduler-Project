#include "Priqueue.h"
#include "queue.h"
int main(int argc, char *argv[])
{
    struct priqueue*pri = (struct priqueue *)malloc(sizeof(struct priqueue));
    struct prinode *node=(struct prinode *)malloc(sizeof(struct prinode));
    struct prinode *node2=(struct prinode *)malloc(sizeof(struct prinode));
    node2->next=NULL;
    node2->process.id=2;
    node2->process.arrivaltime=5;
    node2->process.priority=6;
    node2->process.runningtime=5;
    node2->pri=node2->process.priority;
    node->next=NULL;
    node->process.id=1;
    node->process.arrivaltime=2;
    node->process.priority=3;
    node->process.runningtime=5;
    node->pri=node->process.priority;
    prienqueue(pri,node2);
    prienqueue(pri,node);
    pridequeue(&(node2->process),pri);
    printf("%d  %d  %d   ",node2->process.id-1,node2->process.priority,node2->process.arrivaltime);
    printpriqueue(pri);
    //pri->head->pri=4;
    //pri->actualcount=0;
    // pri->head->next=NULL;
    //prienqueue(pri,node);
    
    free(pri);
    free(node2);   
    
    

}