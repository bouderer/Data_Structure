#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
void dfs(vector<vector<int>>& g, vector<int>&enter, vector<int>& left, int u,int& time){
    enter[u]=time;
    for(int i=0;i<g[u].size();i++){
        int next=g[u][i];
        if(enter[next]==-1){
            dfs(g,enter,left,next,++time);
        }
    }
    left[u]=++time;
    // return seektime+2;
}
int main(){
    int num;
    cin>>num;
    while(num--){
        int n,m;
        cin>>n>>m;
        vector<vector<int>> g(n+1);
        for(int i=0;i<m;i++){
            int u,v;
            cin>>u>>v;
            g[u].push_back(v);
            g[v].push_back(u);
        }
        for(int i=1;i<n+1;i++){
            sort(g[i].begin(),g[i].end(),less<int>());
        }
        vector<int> enter(n+1,-1);
        vector<int> left(n+1,-1);
        int Alltime=0;
        for(int i=1;i<n+1;i++){
            if(left[i]==-1){
                dfs(g,enter,left,i,++Alltime);
            }
        }
        for(int i=1;i<n+1;i++){
            cout<<i<<":"<<enter[i]<<"-"<<left[i]<<endl;
        }
        cout<<"---"<<endl;
    }
    return 0;
}