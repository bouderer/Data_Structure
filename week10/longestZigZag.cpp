//z10-交错路径
#include "TreeNode.h"
#include <iostream>
#include <algorithm>
using namespace std;
int solve(TreeNode *p, int dirct, int &maxlen) {
    // dirct=0，代表p是父节点的左子节点，dirct=1，代表p节点是父节点的右子节点
    //solve依然用于计算交错路径，但是设置一个maxlen用于维护答案。
    if (p) {
        if (dirct == 0) {
            int path = 1 + solve(p->right, 1,maxlen);
            int lpath = solve(p->left, 0, maxlen);
            // 如果继续考虑右节点，这个时候其实就是上面那条代码。而这条路径一定小于path，所以考虑path和lpath比较
            maxlen = max(maxlen, max(lpath, path));
            return path;
        } else {
            int rpath = solve(p->right, 1, maxlen);
            int path=1 + solve(p->left, 0, maxlen);
            maxlen = max(maxlen, max(rpath, path));
            return path;
        }
    }else{
        return 0;
    }
}
int longestZigZag(TreeNode *root) {
    if (!root)
        return 0;
    else {
        int maxlen = 0;
        int lpath=solve(root->left, 0, maxlen);
        int rpath=solve(root->right, 1, maxlen);
        maxlen=max(maxlen,max(rpath,lpath));
        return maxlen;
    }
}
