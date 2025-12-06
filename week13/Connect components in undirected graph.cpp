#include<iostream>
#include<vector>
using namespace std;
int find(vector<int>& root, int v){
    return root[v]==v? v: root[v]=find(root, root[v]);
}
int main(){
    int n,m;
    cin>>n>>m;
    vector<int> root(n+1);
    for(int i=0;i<n+1;i++){
        root[i]=i;
    }
    for(int i=0;i<m;i++){
        int v,y;
        cin>>v>>y;
        int rv=find(root, v);
        int ry=find(root, y);
        if(rv!=ry){
            root[ry]=rv;
        }
    }
    int count=0;
    for(int i=1;i<n+1;i++){
        if(root[i]==i){
            count++;
        }
    }
    cout<<count<<endl;
}