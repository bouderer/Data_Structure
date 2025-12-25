// floyd
#include <iostream>
#include <vector>
#define MAX 1e9
using namespace std;
int main() {
    int N, K, Q;
    cin >> N >> K >> Q;
    int dp[N+1][N+1];
    for (int i = 0; i <= N; i++) {
        for (int j = 0; j <= N; j++) {
            if(i==j) dp[i][j] = 0;
            else dp[i][j]= MAX;
        }
    }
    for (int i = 1; i <= K; i++) {
        int u, v, d;
        cin >> u >> v >> d;
        dp[u][v]=dp[u][v]< d? dp[u][v]:d;
    }
    for(int k=1;k<=N;k++){
        //不同的中间节点
        for(int i=1;i<=N;i++){
            for(int j=1;j<=N;j++){
                if(dp[i][j]>(dp[i][k]+dp[k][j])) dp[i][j]=dp[i][k]+dp[k][j];
            }
        }
    }
    for(int q=0;q<Q;q++){
        int u,v;
        cin>>u>>v;
        cout<<(dp[u][v]==MAX? -1:dp[u][v])<<endl;
    }
    return 0;
}