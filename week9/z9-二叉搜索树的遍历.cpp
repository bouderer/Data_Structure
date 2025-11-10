//z9-二叉搜索树的遍历
#include <iostream>
using namespace std;
class node {
  public:
    int val;
    node *left;
    node *right;
    node() = default;
    node(int val) : val(val), left(nullptr), right(nullptr) {}
    ~node() {
        if (right) {
            delete right;
        }
        if (left) {
            delete left;
        }
    }
};
void inorder(node *&p) {
    if (p == nullptr)
        return;
    if (p->left) {
        inorder(p->left);
    }
    cout << p->val << " ";
    if (p->right) {
        inorder(p->right);
    }
}
void preorder(node *&p) {
    if (p == nullptr)
        return;
    cout << p->val << " ";
    if (p->left) {
        preorder(p->left);
    }
    if (p->right) {
        preorder(p->right);
    }
}
int main() {
    int m;
    while (cin >> m && m != 0) {
        node *root = nullptr;
        for (int i = 0; i < m; i++) { // 构建树
            int num;
            cin >> num;
            if (root == nullptr) {
                root = new node(num);

            } else {
                node *parent = nullptr;
                node* p=root;
                while (p) {
                    parent = p;
                    if (num > p->val)
                        p = p->right;
                    else
                        p = p->left;
                }
                if(num<parent->val){
                    parent->left=new node(num);
                }
                else{
                    parent->right=new node(num);
                }
            }
        }
        inorder(root);
        cout << endl;
        preorder(root);
        cout << endl;
        if (root)
            delete root;
    }
    return 0;
}