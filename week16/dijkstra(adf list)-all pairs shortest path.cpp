// dijkstra
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

const int INF = 1000000000;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, K, Q;
    cin >> N >> K >> Q;

    // 邻接表：adj[u] 存所有从 u 出发的边 (v, w)
    vector<vector<pair<int,int>>> adj(N + 1);

    // 处理重边：同一 (u,v) 可能多次出现，只保留最小权值
    // N<=100 时也可以直接用矩阵先存，再转邻接表（简单）
    vector<vector<int>> w(N + 1, vector<int>(N + 1, INF));
    for (int i = 0; i < K; ++i) {
        int u, v, d;
        cin >> u >> v >> d;
        w[u][v] = min(w[u][v], d);
    }
    for (int u = 1; u <= N; ++u) {
        for (int v = 1; v <= N; ++v) {
            if (w[u][v] != INF) {
                adj[u].push_back({v, w[u][v]});
            }
        }
    }

    // dst[s][v]：从 s 到 v 的最短路
    vector<vector<int>> dst(N + 1, vector<int>(N + 1, INF));

    // 对每个起点 s 跑一次 Dijkstra
    for (int s = 1; s <= N; ++s) {
        vector<int> dist(N + 1, INF);
        dist[s] = 0;

        // 小根堆： (当前距离, 点)
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
        pq.push({0, s});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            // 过期元素（之前推入的旧距离），跳过
            if (d != dist[u]) continue;

            // 遍历 u 的所有出边
            for (auto [v, wuv] : adj[u]) {
                if (dist[v] > d + wuv) {
                    dist[v] = d + wuv;
                    pq.push({dist[v], v});
                }
            }
        }

        // 写回这一行
        for (int v = 1; v <= N; ++v) dst[s][v] = dist[v];
    }

    // 回答查询
    while (Q--) {
        int u, v;
        cin >> u >> v;
        cout << (dst[u][v] == INF ? -1 : dst[u][v]) << '\n';
    }

    return 0;
}
