//z6-Inversion Number
#include<iostream>
#include<vector>
using namespace std;
typedef long long int ll;
ll merge(vector<ll > &num, ll left, ll mid, ll right,vector<ll>& temp){//
    //左右两部分已经排序好，先计算逆序对，在排序。
    ll sum=0;
    ll l=left, r=mid+1,p=left;
    while(l<=mid&&r<=right){
        if(num[r]<num[l]){
            sum+=mid+1-l;
            temp[p++]=num[r++];
        }
        else{
            temp[p++]=num[l++];
        }
    }
    while(l<=mid){
        temp[p++]=num[l++];
    }
    while(r<=right){
        temp[p++]=num[r++];
    }
    for(ll j=left;j<=right;j++ ){
        num[j]=temp[j];
    }
    return sum;
}
ll divide(vector<ll>& num, ll left, ll right, vector<ll >& temp){
    if(right>left){
        ll mid =(left+right)/2;
        ll sum=0;
        sum+=divide(num, left,mid,temp);
        sum+=divide(num, mid+1,right,temp);//算两部分的逆序对
        sum+=merge(num,left,mid,right,temp);//算跨越两部分的逆序对
        return sum;
    }
    else return 0;
}
ll inversion_number(vector<ll>& num){
    vector<ll> temp(num.size());
    return divide(num,0,num.size()-1,temp);

}
int main(){
    ll n;
    while(cin>>n){
        vector<ll> num(n);
        for(ll i=0;i<n;i++){
            cin>>num[i];
        }
        cout<<inversion_number(num)<<endl;
    }
    return 0;
}