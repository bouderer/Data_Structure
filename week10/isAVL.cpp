//z10-判断二叉查找树是否平衡
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
class node {
  public:
    int val;
    int bf;
    node *left;
    node *right;
    node(int val) : val(val), bf(0),left(nullptr),right(nullptr) {}
    node() = default;
    ~node() = default;
};
node *build(vector<int> &num, int begin, int end) {
    if (begin > end) {
        return nullptr;
    } else {
        node *root = new node(num[begin]);
        int right_begin = begin;
        // 找到右子树的子节点
        while (num[right_begin] <= num[begin] && right_begin <= end) {
            right_begin++;
        }
        root->left = build(num, begin + 1, right_begin - 1);
        root->right = build(num, right_begin, end);
        return root;
    }
}
int H(node *root) {
    if (root==nullptr) {
        return 0;
    } else {
        int lheight = H(root->left);
        int rheight = H(root->right);
        int height = max(lheight, rheight) + 1;
        root->bf = lheight - rheight;
        return height;
    }
}
bool balance(node *root) {
    if(!root) return true;
    if (balance(root->left) && balance(root->right)) {
        return abs(root->bf) <= 1;
    } else {
        return false;
    }
}
int main() {
    int m;
    cin >> m;
    while (m--) {
        vector<int> num;
        int n;
        cin >> n; // 读取数量
        for (int i = 0; i < n; i++) {
            // 读取数组
            int a;
            cin >> a;
            num.push_back(a);
        }
        // 构建树
        node *root = build(num, 0, n - 1);
        // 计算每个节点的高度
        H(root);
        if (balance(root)) {
            cout << "YES" << endl;
        } else {
            cout << "NO" << endl;
        }
    }
}