#pragma once
#include "Tree.h"
#include "headers.h"

// Entry Definitions and Functions
typedef struct Entry
{
    int Process_ID;
    TreeNode* Block;
    int size; // Size of allocated block
    struct Entry* next;
} Entry;

void Initialize_Entry(Entry *e, int pid, TreeNode* block)
{
    e->Process_ID = pid;
    e->Block = block;
    e->size = block->size;
    e->next = NULL;
}

// Memory Table allocated as a linked list
// Memory Table Definitions and Functions
typedef struct Memory_Table
{
    Entry* head;
} Memory_Table;

void AddEntry(Memory_Table* mt, Entry* e)
{
    if (mt->head == NULL)
    {
        mt->head = e;
        return;
    }
    else
    {
        e->Block->status = 1; // set it as allocated
        e->next = mt->head;
        mt->head = e;
    }
}


void FreeMT(Memory_Table* mt)
{
    if (mt->head == NULL)
    {
        return;
    }
    else
    {
        Entry* e = mt->head;
        mt->head = mt->head->next;
        while (mt->head)
        {
            free(e);
            e = mt->head;
            mt->head = mt->head->next;
        }
        free(e);
    }
}

void printMemTable(Memory_Table* mt)
{
    Entry* e = mt->head;
    if (!e)
    {
        printf("Memory Table is empty\n");
        return;
    }
    else
    {
        while (e)
        {
            printf("Memory: From: %d, To: %d\n", e->Block->from, e->Block->to);
            e = e->next;
        }
    }
}


TreeNode* RemoveFromMemTable(Memory_Table* mt, int pid)
{
    //check for merge
    Entry* eptr = mt->head;
    TreeNode* node;
    if (eptr == NULL)
    {
        return NULL;
    }
    if (eptr->Process_ID == pid)
    {
        // remove head
        mt->head = mt->head->next;
        eptr->Block->status = 0;
        node = eptr->Block;
        free(eptr);
        return node;
    }
    // More than one entry in free memory table 
    Entry* next = eptr->next;
    while (next)
    {
        if (next->Process_ID == pid)
        {
            node = next->Block;
            node->status = 0;
            eptr->next = next->next;
            free(next);
            return node;
        }
        eptr = eptr->next;
        next = eptr->next;
    }
    return NULL;
}
