#ifndef TREENODE_H_INCLUDED
#define TREENODE_H_INCLUDED

struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
};

int longestZigZag(TreeNode* root);

#endif // TREENODE_H_INCLUDED
