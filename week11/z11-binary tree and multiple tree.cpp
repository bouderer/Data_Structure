//z11-多叉树与二叉树
#include<iostream>
#include<queue>
#include<vector>
using namespace std;
int main(){
    int n;
    cin>>n;
    vector<int> father(n);//当前索引对应的父节点
    vector<int> leftchild(n);//当前节点的左子节点
    vector<int> rightchild(n);//当前节点的右子节点
    vector<int> lastchild(n);//当前节点的最后一个右子节点
    father[0]=0;
    rightchild[0]=0;
    lastchild[0]=0;
    //leftchild[0]待定
    for(int i=1;i<n;i++){
        int Father;
        cin>>Father;
        father[i]=Father;//索引为i的父节点是father
        if(leftchild[Father]==0){
            leftchild[Father]=i+1;
        }
        if(lastchild[Father]!=0){
            int brother=lastchild[Father];//同一个父亲的左兄弟
            rightchild[brother]=i+1;
        }
        lastchild[Father]=i+1;
    }
    queue<int> result;
    result.push(1);//放入根节点
    while(result.empty()==0){
        int cur=result.front();
        if(cur!=0){
            cout<<cur<<" ";
            result.push(leftchild[cur]);
            result.push(rightchild[cur]);
        }
        result.pop();
    }
    return 0;
}