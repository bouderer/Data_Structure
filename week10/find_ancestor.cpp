//z10-完全二叉树-最近公共祖先
#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
void find(vector<int>& path,int root,int num){
    while(num!=root){
        path.push_back(num);
        num/=2;
    }
    path.push_back(root);
    reverse(path.begin(),path.end());
}
int main(){
    int n;
    cin>>n;
    while(n--){
        int x,y;
        cin>>x>>y;
        vector<int> xpath;
        vector<int> ypath;
        find(xpath,1,x);
        find(ypath,1,y);
        int i=0;
        while(xpath[i]==ypath[i]){
            i++;
        }
        cout<<xpath[i-1]<<endl;
        
    }
    return 0;
}