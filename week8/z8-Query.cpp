#include<iostream>
#include"query.h"
#include<string>
#include<unordered_map>
using namespace std;
void query(string A[], int n, string B[], int m){
    unordered_map<string,int> hash;
    for(int i=0;i<n;i++){
        if(hash.count(A[i])==0) hash.emplace(A[i],1);
        else hash[A[i]]++;
    }
    for(int i=0;i<m;i++){
        if(hash.count(B[i])!=0){
            cout<<B[i]<<endl;
        }
    }
}
