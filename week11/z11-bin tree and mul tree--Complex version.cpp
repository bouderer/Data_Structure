#include <iostream>
#include <unordered_map>
#include <queue>
using namespace std;
class node {
  public:
    int pos;
    node *left;
    node *right;
    node() = default;
    node(int pos) : pos(pos), left(nullptr), right(nullptr) {}
    ~node(){
        if(left) delete left;
        if(right) delete right;
    }
};
node *solve(vector<int> &num, unordered_map<int, queue<int>> &hash, int idx) {
    node *root = new node(idx + 1);
    // 找左子节点
    if (hash.count(idx + 1)) {
        int leftchild = hash[idx + 1].front();
        hash[idx + 1].pop();
        if (hash[idx + 1].empty()) {
            hash.erase(idx + 1);
        }
        root->left = solve(num, hash, leftchild);
    }
    //右子节点，也就是兄弟节点
    if (hash.count(num[idx])) {
        int rightchild = hash[num[idx]].front();
        hash[num[idx]].pop();
        if (hash[num[idx]].empty()) {
            hash.erase(num[idx]);
        }
        root->right = solve(num, hash, rightchild);
    }
    return root;
}
int main() {
    int n;
    cin >> n;
    vector<int> num(n);
    unordered_map<int, queue<int>> hash; // father's pos->child's idx
    num[0] = 0;
    for (int i = 1; i < n; i++) {
        cin >> num[i];
        hash[num[i]].push(i);
    }
    node *root = solve(num, hash, 0);
    queue<node *> result;
    result.push(root);
    while (!result.empty()) {
        node *cur = result.front();
        if (cur) {
            result.push(cur->left);
            result.push(cur->right);
            cout << cur->pos << " ";
        }
        result.pop();
    }
    delete root;
    return 0;
}
//首次使用int queue的hash
//层序遍历
//树形结构释放