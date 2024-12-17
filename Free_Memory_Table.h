#pragma once
#include"Tree.h"
#include"headers.h"

// Free_Entry Definitions and Functions
typedef struct Free_Entry
{
    TreeNode* Block;
    int size; // Size of allocated block
    struct Free_Entry* next;
} Free_Entry;

void Initialize_Free_Entry(Free_Entry *e, TreeNode* block)
{
    e->Block = block;
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
    if(fmt->head == NULL || fmt->head->size > e->size) // insert at the beginning
    {
        e->next=fmt->head;
        fmt->head = e;
        return;
    }
    Free_Entry* current = fmt->head;
    while(current->next != NULL && (current->next->size)<=(e->size))
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


