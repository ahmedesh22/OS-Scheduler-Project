#pragma once
#include "headers.h"



// Tree Node Definitions and Functions
typedef struct TreeNode 
{
    int size;
    int status; // (0 --> Free) (1 --> Allocated)
    int from;
    int to; 
    struct TreeNode* parent;
    struct TreeNode* Lchild;
    struct TreeNode* Rchild;
    struct TreeNode* Buddy; // A pointer to the block next to it. (its sibling)
} TreeNode;

void Initialize_TreeNode(TreeNode *t, int size, int status, int from, int to, TreeNode *parent)
{
    t->size = size;
    t->status = status;
    t->from = from;
    t->to = to;
    t->parent = parent;
    t->Lchild = NULL;
    t->Rchild = NULL;
    // The parent node will bre responsible for setting the buddy pointer
}


// Tree Definitions and Functions
typedef struct BinaryTree
{
    TreeNode *root;
} BinaryTree;

void AddTreeNodes (TreeNode* parentnode) // will be needed if we want to split a memory block
{
    TreeNode* lchild = (TreeNode*) malloc(sizeof(TreeNode));
    TreeNode* rchild = (TreeNode*) malloc(sizeof(TreeNode));

    int mid = parentnode->from + (parentnode->to - parentnode->from) / 2; //== (from+to) / 2
    
    // initialize nodes with half the size of the parent and initially set them both as free (not allocated yet)
    Initialize_TreeNode(lchild, parentnode->size / 2, 0, parentnode->from, mid, parentnode);
    Initialize_TreeNode(rchild, parentnode->size / 2, 0, mid + 1, parentnode->to, parentnode);
    parentnode->status=2;//state 2 is split
    parentnode->Lchild = lchild;
    parentnode->Rchild = rchild;
    // setting the buddy pointer
    lchild->Buddy = parentnode->Rchild;
    rchild->Buddy = parentnode->Lchild;

  
}

void AllocateTreeNode (TreeNode* node)
{
    node->status = 1; // allocated
    // Remove it from the free memory table
    // put it in the memory table
}

void Initialize_Tree (BinaryTree* tree)
{
    tree->root = (TreeNode*) malloc(sizeof(TreeNode));
    Initialize_TreeNode(tree->root, 1024, 0, 0, 1023, NULL); // Initialize the biggest memory block (1024) and initially set its status to be free
}


void FreeTree(TreeNode* root) // Call at the end of simulation to ensure that we freed any dynamically allocated memory
{
    // Base case
    if (root == NULL)
    {
        return;
    }

    // Recur on the left subtree
    FreeTree(root->Lchild);
  
    // Recur on the right subtree
    FreeTree(root->Rchild);
    
    free(root);
}



