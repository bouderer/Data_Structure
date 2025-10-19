//z6-明明的随机数
#include<iostream>
using namespace std;
int main(){
    int n;
    while(cin>>n){
        int bucket[1001]={0};
        int sum=0;
        for(int i=0;i<n;i++){
            int m;
            cin>>m;
            if(bucket[m]!=1){
                sum++;
                bucket[m]=1;
            }
        }
        cout<<sum<<endl;
        for(int i=1;i<=1000;i++){
            if(bucket[i]==1) cout<<i<<" ";
        }
    }
    return 0;
}