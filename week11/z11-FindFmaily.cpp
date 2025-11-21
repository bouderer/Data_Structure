//z11-家族查询
#include<iostream>
#include<vector>
using namespace std;
int find(int a,vector<int>& parent){
    return parent[a]==a? a:(parent[a]=find(parent[a],parent));
}
int main(){
    int T;
    cin>>T;
    while(T--){
        int n,m;
        cin>>n>>m;
        vector<int> parent(n+1);
        for(int i=0;i<n+1;i++){
            parent[i]=i;
        }
        while(m--){
            int u,v;
            cin>>u>>v;
            int ru=find(u,parent);
            int rv=find(v,parent);
            parent[rv]=ru;
        }
        int q;
        cin>>q;
        while(q--){
            int u, v;
            cin>>u>>v;
            cout<<(find(u,parent)==find(v,parent)? "Yes":"No")<<endl;
        }
    }
    return 0;
}