//z11-连通性问题
#include<iostream>
#include<vector>
using namespace std;
#define MAXLEN 100001
int main(){
    int m,n;
    vector<int> father(MAXLEN);
    for(int i=0;i<MAXLEN;i++){
        father[i]=i;
    }
    while(cin>>m>>n){
        int rm=m,rn=n;
        int heightm=0,heightn=0;
        while(father[rm]!=rm){
            rm=father[rm];
            heightm++;
        }
        while(father[rn]!=rn){
            rn=father[rn];
            heightn++;
        }
        if(rm!=rn){
            cout<<m<<" "<<n<<endl;
            if(heightm<heightn){
                father[rm]=rn;
            }else{
                father[rn]=rm;
            }
        }else{
            while(m!=rm){
                int tempm=m;
                m=father[m];
                father[tempm]=rm;
            }
            while(n!=rn){
                int tempn=n;
                n=father[n];
                father[tempn]=rn;
            }
        }
    }
    return 0;
}
