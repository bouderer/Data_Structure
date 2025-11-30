//z12-哈夫曼树编码
#include <iostream>
#include <vector>
#include <queue>
#include<algorithm>
using namespace std;
class node {
  public:
    char chr;
    int frq;
    node *left;
    node *right;
    node() = default;
    node(int frq, char chr = 0)
        : frq(frq), chr(chr), left(nullptr), right(nullptr) {}
    ~node() {
        delete left;
        delete right;
    }
    int leaf() { return (!left) && (!right); }
};
struct cmp {
  public:
    bool operator()(node *a, node *b) {
        if (a->frq == b->frq) {
            return a->chr > b->chr;
        }
        return a->frq > b->frq;
    }
};
node *build(priority_queue<node *, vector<node *>, cmp> &que) {
    while (que.size() > 1) {
        node *right = que.top();
        que.pop();
        node *left = que.top();
        que.pop();
        node *root = new node(left->frq + right->frq,max(left->chr,right->chr));
        
        root->left = left;
        root->right = right;
        que.push(root);
    }
    node *root = que.top();
    que.pop();
    return root;
}
void display(node *root) {
    if (root->leaf()) {
        cout << root->chr << endl;
        return;
    }
    display(root->left);
    display(root->right);
}
int main() {
    int n;
    cin >> n;
    priority_queue<node *, vector<node *>, cmp> que;
    for (int i = 0; i < n; i++) {
        char chr;
        int frq;
        cin >> chr >> frq;
        node *root = new node(frq, chr);
        que.push(root);
    }
    node *root = build(que);
    display(root);
    delete root;
    return 0;
}