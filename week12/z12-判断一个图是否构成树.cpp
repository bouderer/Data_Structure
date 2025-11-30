//z12-判断一个图是否构成树
#include <iostream>
#include <vector>
#include <string>
using namespace std;
int find(vector<int> &root, int i)
{
    return root[i] == i ? i : root[i] = find(root, root[i]);
}
void func() {}
int main()
{
    int num;
    cin >> num;
    while (num--)
    {
        int n, m;
        cin >> n >> m;
        vector<int> root(n + 1);
        int hascycle = 0;
        for (int i = 0; i < n + 1; i++)
        {
            root[i] = i;
        }
        for (int i = 0; i < m; i++)
        {
            int u, v;
            cin >> u >> v;
            int ru = find(root, u);
            int rv = find(root, v);
            if (ru == rv)
            {
                hascycle = 1;
            }
            else
            {
                root[ru] = rv;
            }
        }
        if (!hascycle && m == n - 1)
        {
            // 根据树的性质，一共有n个节点，就会有n-1条边
            // 如果符合条件，且无环，则是树
            cout << "YES" << endl;
        }
        else
        {
            cout << "NO" << endl;
        }
    }
    return 0;
}
