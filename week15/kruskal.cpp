//kruskal
//从边的角度出发。因为所有的权重都来自于边，所以只需要遍历所有边，并跳过某些情况即可
#include<iostream>
#include<vector>
#include<queue>
using namespace std;
struct edge{
    int u;
    int v;
    int weight;
    edge()=default;
    edge(int u,int v,int weight):u(u),v(v),weight(weight){}
    bool operator<(const edge& other) const{
        return this->weight<other.weight;
    }
};
int find(vector<int>& root, int u){
    return root[u]==u ? u : root[u]=find(root, root[u]);
}
int main(){
    int num;
    cin>>num;
    while(num--){
        int n,m;
        cin>>n>>m;
        priority_queue<edge,vector<edge>,less<edge>> e;
        //储存每一个点的根节点，用于判断是否同一棵树
        vector<int> root(n);
        for(int i=0;i<n;i++){
            root[i]=i;
        }
        //初始化数据，用edge类储存边，用priority_queue储存edge
        for(int i=0;i<m;i++){
            int x,y,d;
            cin>>x>>y>>d;
            e.push(edge(x,y,d));
        }
        int weight=0;
        while(!e.empty()){
            edge crt=e.top();
            e.pop();
            int u=crt.u, v=crt.v, ru, rv; 
            ru = find(root, u);
            rv = find(root, v);
            if(ru!=rv){
                weight+=crt.weight;
                root[ru]=rv;
            }else{
                continue;
            }
        }
        cout<<weight<<endl;
    }
    return 0;
}