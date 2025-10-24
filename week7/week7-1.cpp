//z7-检查一个序列是否构成堆
#include<iostream>
#include<vector>
using namespace std;
int main(){
    int n;
    cin>>n;
    vector<int> num(n);
    for (int i=0;i<n;i++){
        cin>>num[i];
    }
    int max=1,min=1;
    for(int i=0;i<=n/2-1;i++){
        int left=2*i+1;
        int right = 2*i+2;
        if(left<n&&num[left]>num[i]) max=0;
        if(right<n&&num[right]>num[i]) max=0;
        if(left<n&&num[left]<num[i]) min=0;
        if(right<n&&num[right]<num[i]) min=0;
    }
    if(max&&min) cout<<"both"<<endl;
    else if(max) cout<<"max heap"<<endl;
    else if(min) cout<<"min heap"<<endl;
    else cout<<"no"<<endl;
    return 0;
}