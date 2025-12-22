//prim
#include<iostream>
#include<vector>
#include<queue>
using namespace std;
struct cmp{
    bool operator()(const pair<int,int>& a,const pair<int,int>& b){
        return a.second<b.second;
    }
};
int main(){
    int num;
    cin>>num;
    while(num--){
        int n,m;
        cin>>n>>m;
        //邻接表
        vector<vector<pair<int,int>>> list(n);
        //访问记录
        vector<bool> vst(n,false);
        for(int i=0;i<m;i++){
            int x,y,d;
            cin>>x>>y>>d;
            list[x].push_back(make_pair(y,d));
            list[y].push_back(make_pair(x,d));
        }
        int weight=0;
        for(int i=0;i<n;i++){
            if (vst[i]==false){
                //找到一个连通分支，构建最大生成树
                priority_queue<pair<int,int>,vector<pair<int,int>>, cmp> q;
                q.push(make_pair(i,0));
                while(!q.empty()){
                    //先把当前节点取出来
                    pair<int,int> crt = q.top();
                    q.pop();
                    //如果当前节点已经被访问过，需要跳过
                    if(vst[crt.first]==true) continue;
                    //否则
                    vst[crt.first]=true;
                    weight+=crt.second;
                    for(auto it:list[crt.first]){
                        //将所有邻接节点压入队列
                        q.push(it);
                    }
                }
            }
        }
        cout<<weight<<endl;
    }
}