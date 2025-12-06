#include <iostream>
#include <vector>
#include <list>
using namespace std;
void dfs(vector<list<int>>& g, vector<bool>& visited, int u){
    visited[u]=true;
    for(auto v: g[u]){
        if(!visited[v]){
            dfs(g,visited,v);
        }
    }
}
int main() {
    int n, m;
    while (cin >> n >> m) {
        vector<list<int>> g(n);
        for(int i=0;i<m;i++){
            int u,v;
            cin>>u>>v;
            g[u].push_back(v);
        }
        vector<bool> visited(n,false);
        dfs(g,visited,0);
        if(visited[n-1]){
            cout<<"I can post the letter"<<endl;
        }else{
            cout<<"I can't post the letter"<<endl;
        }
    }
}