//z9-Play with Tree
#include <iostream>
#include "play.h"
using namespace std;
void query(const Node *root, int &size, int &height) {
    if (root == nullptr) {
        size = 0;
        height = 0;
        return;
    } else {
        int lsize = 0, rsize = 0;
        int lheight = 0, rheight = 0;
        query(root->lc, lsize, lheight);
        query(root->rc, rsize, rheight);
        size = lsize + rsize + 1;
        height = lheight > rheight ? lheight : rheight;
        height += 1;//算上自己的高度
    }
}