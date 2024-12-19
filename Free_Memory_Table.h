#pragma once
#include"headers.h"
#include "Tree.h"

// Free_Entry Definitions and Functions
typedef struct Free_Entry
{
    TreeNode* node;
    int size; // Size of allocated block
    struct Free_Entry* next;
} Free_Entry;

void Initialize_Free_Entry(Free_Entry *e, TreeNode* block)
{
    e->node = block;
    e->size = block->size;
    e->next = NULL;
}

// Memory Table allocated as a linked list
// Memory Table Definitions and Functions
typedef struct Free_Memory_Table
{
    Free_Entry* head;
} Free_Memory_Table;

void AddFreeEntry(Free_Memory_Table* fmt, Free_Entry* e)
{
    if(fmt->head == NULL || fmt->head->node->from > e->node->from) // insert at the beginning
    {
        e->next=fmt->head;
        fmt->head = e;
        return;
    }
    Free_Entry* current = fmt->head;
    while(current->next != NULL && (current->next->node->from) < (e->node->from))
    {
        current=current->next;
    }
    e->next=current->next;
    current->next=e;
}


void FreeFMT(Free_Memory_Table* fmt)
{
    if (fmt->head == NULL)
    {
        return;
    }
    else
    {
        Free_Entry* fe = fmt->head;
        fmt->head = fmt->head->next;
        while (fmt->head)
        {
            free(fe);
            fe = fmt->head;
            fmt->head = fmt->head->next;
        }
        free(fe);
    }
}


Free_Entry* CanAllocate(Free_Memory_Table* fmt,  int size) // size given to check if we can allocate it or not
{
    // loop on the free memory table list to check if we can allocate it or not
    Free_Entry* fe = fmt->head;
    while (fe)
    {
        if (size <= fe->size)
        {
            return fe; // Can be allocated
        }
        fe = fe->next;
    }
    return NULL; // No Location to allocate into
}

TreeNode* RemoveFromFreeMemTable(Free_Memory_Table* fmt, Free_Entry* fe)
{
    Free_Entry* fptr = fmt->head;
    TreeNode* tptr;
    if (fptr == NULL)
    {
        return NULL;
    }
    if (fptr->node->to == fe->node->to)
    {
        // remove head
        tptr=fptr->node;
        fmt->head = fmt->head->next;
        free(fptr);
        return tptr;
    }
    // More than one entry in free memory table 
    Free_Entry* next = fptr->next;
    while (next)
    {
        if (next->node->to == fe->node->to)
        {
            tptr=next->node;
            fptr->next = next->next;
            free(next);
            return tptr;
        }
        fptr = fptr->next;
        next = fptr->next;
    }
    return NULL;
}


void printFreeMemTable(Free_Memory_Table* fmt)
{
    Free_Entry* e = fmt->head;
    if (!e)
    {
        printf("Free Memory Table is empty\n");
        return;
    }
    else
    {
        while (e)
        {
            printf("Free: From: %d, To: %d\n", e->node->from, e->node->to);
            e = e->next;
        }
    }
}

TreeNode* Split(Free_Memory_Table* fmt, TreeNode* parent, int psize)
{
    AddTreeNodes(parent);
    TreeNode* lchild = parent->Lchild;
    // put right Child in free memory table
    Free_Entry* fe = (Free_Entry*) malloc(sizeof(Free_Entry));
    Initialize_Free_Entry(fe, parent->Rchild);
    AddFreeEntry(fmt, fe);
    while (CheckSize(psize, lchild->size) == 1)
    {
        AddTreeNodes(lchild);
        // put right Child in free memory table
        Free_Entry* free_e = (Free_Entry*) malloc(sizeof(Free_Entry));
        Initialize_Free_Entry(free_e, lchild->Rchild);
        AddFreeEntry(fmt, free_e);
        lchild = lchild->Lchild;
    }
    return lchild;
}
TreeNode* Merge(Free_Memory_Table* fmt,TreeNode*node)
{
    Free_Entry*fe;
    if(!node)
    {
        return NULL;
    }
    if(!fmt->head)
    {
        return node;
    }
    while(node->status==0 && node->parent &&node->Buddy->status==0)
    {
        fe=(Free_Entry*)malloc(sizeof(Free_Entry));
        Initialize_Free_Entry(fe,node->Buddy);
        RemoveFromFreeMemTable(fmt,fe);
        node=node->parent;
        free(node->Lchild);
        free(node->Rchild);
        node->Lchild=NULL;
        node->Rchild=NULL;
        node->status=0;
    }
    return node;
}