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
