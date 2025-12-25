// dijkstra
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#define MAX 1e9
using namespace std;
struct cmp {
    bool operator()(pair<int, int> &a, pair<int, int> &b) {
        return a.second > b.second;
    }
};
int main() {
    int N, K, Q;
    cin >> N >> K >> Q;
    int dst[N + 1][N + 1];
    int near[N + 1][N + 1];
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            near[i][j] = 0;
            if (i == j)
                dst[i][j] = 0;
            else
                dst[i][j] = MAX;
        }
    }
    for (int i = 1; i <= K; i++) {
        int u, v, d;
        cin >> u >> v >> d;
        if(near[u][v]==0||near[u][v]>d) near[u][v]=d;
    }
    for (int i = 1; i <= N; i++) {
        priority_queue<pair<int, int>, vector<pair<int, int>>, cmp> q;
        vector<bool> visited(N + 1, false);
        q.push(make_pair(i, 0));
        while (!q.empty()) {
            auto cur_pair = q.top();
            q.pop();
            int c = cur_pair.first;
            if (visited[c]) continue;
            visited[c] = true;
            for (int j = 1; j <= N; j++) {
                if (near[c][j] && !visited[j]) {
                    // 更新路径距离
                    if (dst[i][j] > dst[i][c] + near[c][j]) {
                        dst[i][j] = dst[i][c] + near[c][j];
                        q.push(make_pair(j, dst[i][j]));
                    }
                }
            }
        }
    }
    for (int q = 0; q < Q; q++) {
        int u, v;
        cin >> u >> v;
        cout << (dst[u][v] == MAX ? -1 : dst[u][v]) << endl;
    }
    return 0;
}