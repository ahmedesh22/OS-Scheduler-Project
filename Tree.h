#pragma once
#include "headers.h"


// Tree Node Definitions and Functions
typedef struct TreeNode 
{
    int size;
    int status; // (0 --> Free) (1 --> Allocated)
    struct TreeNode* Lchild;
    struct TreeNode* Rchild;
    struct TreeNode* Buddy; // A pointer to the block next to it. (its sibling)
} TreeNode;

void Initialize_TreeNode(TreeNode *t, int size, int status)
{
    t->size = size;
    t->status = status;
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
    
    // initialize nodes with half the size of the parent and initially set them both as free (not allocated yet)
    Initialize_TreeNode(lchild, parentnode->size / 2, 0);
    Initialize_TreeNode(rchild, parentnode->size / 2, 0);
    
    // setting the buddy pointer
    lchild->Buddy = parentnode->Rchild;
    rchild->Buddy = parentnode->Lchild;

    parentnode->Lchild = lchild;
    parentnode->Rchild = rchild;
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
    Initialize_TreeNode(tree->root, 1024, 0); // Initialize the biggest memory block (1024) and initially set its status to be free
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

