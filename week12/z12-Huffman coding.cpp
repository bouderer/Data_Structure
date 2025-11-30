//z12-Huffman coding
#include<iostream>
#include<queue>
#include<vector>
using namespace std;
int main(){
    int n;
    cin>>n;
    priority_queue<int,vector<int>,greater<int>> que;
    for(int i=0;i<n;i++){
        int frq;
        char chr;
        cin>>chr>>frq;
        que.push(frq);
    }
    int result=0;
    while(que.size()>1){
        int a=que.top();
        que.pop();
        int b=que.top();
        que.pop();
        int c=a+b;
        result+=c;
        que.push(c);
    }
    cout<<result<<endl;
}