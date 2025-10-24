#include <iostream>
#include"heap.h"
using namespace std;
void heap::push(int a){
    n++;
    h[n]=a;//放入最后的节点
    int i=n;
    while(i>1){
        int father=i/2;
        if(h[father]>h[i]){
            int temp=h[father];
            h[father]=h[i];
            h[i]=temp;
        }
        i=father;

    }
}
void heap::pop(){
    if(n>=1){
        h[1]=h[n];
        h[n--]=0;
        int i=1;
        while(true){
            int left=2*i;
            int right=2*i+1;
            if(left<=n){
                if(right<=n){//有两个节点
                    if(h[i]>h[left]&&h[i]>h[right]){
                        if(h[left]>h[right]){
                            int temp=h[right];
                            h[right]=h[i];
                            h[i]=temp;
                            i=right;
                        }
                        else{
                            int temp=h[left];
                            h[left]=h[i];
                            h[i]=temp;
                            i=left;
                        }
                    }
                    else if(h[i]>h[left]){
                            int temp=h[left];
                            h[left]=h[i];
                            h[i]=temp;
                            i=left;
                    }
                    else if(h[i]>h[right]){
                            int temp=h[right];
                            h[right]=h[i];
                            h[i]=temp;
                            i=right;
                    }
                    else break;
                }
                else{
                    if(h[left]<h[i]){
                        int temp=h[i];
                        h[i]=h[left];
                        h[left]=temp;
                    }
                    break;
                }
            }
            else break;
        }
    }
}
