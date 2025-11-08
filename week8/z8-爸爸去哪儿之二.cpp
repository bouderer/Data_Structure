//z8-爸爸去哪儿之二
#include<iostream>
#include<string>
#include<iomanip>
#include<cmath>
using namespace std;
int main(){
    int n,m;
    while(cin>>n>>m){
        double count=0;
        string house[m];
        for(int i=0;i<m;i++){
            house[i]="NULL";
        }
        for(int i=0;i<n;i++){
            string name;
            cin>>name;
            int numName=0;
            for(int j=0;j<name.length();j++){
                numName+=name[j]-'a'+1;
            }
            int addr=numName%m;
            int idx=addr;
            int times=0;
            while(true){
                times++;
                if(house[idx]=="NULL"){
                    house[idx]=name;
                    break;
                }else{
                    idx=addr+pow(times,2);
                    idx%=m;
                }
            }
            count+=times;
        }
        for(int i=0;i<m;i++){
            cout<<i<<':'<<house[i]<<endl;
        }
        cout<<fixed<<setprecision(3);
        cout<<count/n<<endl;
    }
}