#include<iostream>
#include<vector>
using namespace std;
int find(int v, vector<int>& root){
    return v==root[v]? v : root[v]=find(root[v],root);
}
int main(){
    int T;
    cin>>T;
    while(T--){
        int n,m;
        cin>>n>>m;
        vector<int> d(n+1,0);
        vector<int> root(n+1,0);
        for(int i=0;i<n+1;i++){
            root[i]=i;
        }
        for(int i=0;i<m;i++){
            int u,v;
            cin>>u>>v;
            d[v]++;
            d[u]++;
            int ru,rv;
            ru=find(u,root);
            rv=find(v,root);
            if(ru!=rv){
                root[ru]=rv;
            }
        }
        int cnt=0;
        int TreeNum=0;
        for(int i=1;i<n+1;i++){
            cnt+=d[i]%2;
            if(root[i]==i){
                TreeNum++;
            }
        }
        if(TreeNum!=1){
            cout<<"Neither"<<endl;
        }
        else if(cnt==0){
            cout<<"Euler Circuit"<<endl;
        }else if(cnt==2){
            cout<<"Euler Path"<<endl;
        }else{
            cout<<"Neither"<<endl;
        }
    }
}