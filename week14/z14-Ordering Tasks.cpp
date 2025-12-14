#include<iostream>
#include<vector>
#include<queue>
// #include<algorithm>
using namespace std;
int main(){
    int T;
    cin>>T;
    while(T--){
        int n,m;
        cin>>n>>m;
        vector<vector<int>> g(n+1);
        vector<int> in(n+1,0);
        priority_queue<int,vector<int>,greater<int>> q;
        vector<int> rst;
        for(int i=0;i<m;i++){
            int u,v;
            cin>>u>>v;
            g[u].push_back(v);
            in[v]++;
        }
        for(int i=1;i<n+1;i++){
            if(in[i]==0){
                q.push(i);
            }
        }
        while(!q.empty()){
            int c=q.top();
            q.pop();
            for(auto next:g[c]){
                in[next]--;
                if(in[next]==0){
                    q.push(next);
                }
            }
            rst.push_back(c);
        }
        for(int i=0;i<n;i++){
            cout<<rst[i]<<" ";
        }
        cout<<endl;
    }
    return 0;
}