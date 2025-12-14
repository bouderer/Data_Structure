#include <iostream>
#include <vector>
#include <queue>
#include <list>
using namespace std;
int main() {
    int n, m;
    cin >> n >> m;
    vector<list<int>> g(n + 1);
    vector<int> in(n + 1, 0);
    queue<int> q;
    list<int> result;
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        in[v]++;
        g[u].push_back(v);
    }
    for (int i = 1; i < n + 1; i++) {
        if (!in[i])
            q.push(i);
    }
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        for (auto next : g[cur]) {
            if (--in[next] == 0)
                q.push(next);
        }
        result.push_back(cur);
    }
    // for (auto a : result) {
    //     cout << a << " ";
    // }
    // cout << endl;

    cout << (result.size() == n ? 1 : 0) << endl;
}