#include<iostream>
#include<vector>
#include<queue>
using namespace std;

using node = pair<int, pair<int,int>>;

int main(){
    int N;
    cin >> N;
    while(N--){
        int rows, cols;
        cin >> rows >> cols;

        vector<vector<int>> weight(rows, vector<int>(cols));
        for(int i=0;i<rows;i++)
            for(int j=0;j<cols;j++)
                cin >> weight[i][j];

        int s_row, s_col, e_row, e_col;
        cin >> s_row >> s_col >> e_row >> e_col;
        --s_row; --s_col; --e_row; --e_col;

        const int INF = 1e9;
        priority_queue<node, vector<node>, greater<node>> q;
        vector<vector<int>> dst(rows, vector<int>(cols, INF));

        dst[s_row][s_col] = weight[s_row][s_col];
        q.push(node{dst[s_row][s_col], {s_row, s_col}});

        int dr[4] = {-1, 0, 1, 0};
        int dc[4] = {0, 1, 0, -1};

        while(!q.empty()){
            auto cur = q.top(); q.pop();
            int w = cur.first, r = cur.second.first, c = cur.second.second;
            if(w != dst[r][c]) continue;
            if(r == e_row && c == e_col) break;

            for(int k=0;k<4;k++){
                int nr = r + dr[k], nc = c + dc[k];
                if(nr<0 || nr>=rows || nc<0 || nc>=cols) continue;
                int nd = w + weight[nr][nc];
                if(nd < dst[nr][nc]){
                    dst[nr][nc] = nd;
                    q.push(node{nd, {nr, nc}});
                }
            }
        }

        cout << dst[e_row][e_col] << endl;
    }
}
