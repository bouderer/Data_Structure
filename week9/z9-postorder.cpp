//z9-postorder
#include <iostream>
#include <vector>
using namespace std;
class node {
  public:
    int val;
    node *left;
    node *right;
    node() = default;
    node(int val) : val(val), left(nullptr), right(nullptr) {}
    ~node() = default;
};
node *biuldtree(const vector<int> &pre, const vector<int> &in, int idxPre,
                int left, int right, const vector<int>& Findidx) {
    if (left > right) {
        return nullptr;
    }
    // 在in中找到pre[idxPre]的位置
    int idxIn = Findidx[pre[idxPre]];
    node *root = new node(pre[idxPre]);
    int Lright = idxIn - 1;         // 左子树可以取到的右边界索引值
    int Lleft = left;               // 左子树可以取到的左边界索引值
    int Rright = right;             // 右子树可以取到的右边界索引值
    int Rleft = idxIn + 1;          // 右子树可以取到的左边界索引值
    int Lsize = Lright - Lleft + 1; // 左子树的节点数量
    root->left = biuldtree(pre, in, idxPre + 1, Lleft, Lright,Findidx);
    // 默认前缀表达式下，根节点下一个就是左子树的节点。如果没有左子树，会自动返回nullptr
    root->right = biuldtree(pre, in, idxPre + Lsize + 1, Rleft, Rright,Findidx);
    // 需要跨过左子树的节点数量，才是右子树的根节点
    return root;
}
void post(node *root) {
    if (root == nullptr)
        return;
    else {
        post(root->left);
        post(root->right);
        cout << root->val << " ";
    }
}
int main() {
    int n;
    cin >> n;
    vector<int> pre(n);
    vector<int> in(n);
    vector<int> Findidx(n);
    for (int i = 0; i < n; i++) {
        cin >> pre[i];
    }
    for (int i = 0; i < n; i++) {
        cin >> in[i];
    }
    for (int i=0;i<n;i++){
        Findidx[in[i]]=i;
    }
    node *root = biuldtree(pre, in, 0, 0, n - 1,Findidx);
    post(root);
    return 0;
}