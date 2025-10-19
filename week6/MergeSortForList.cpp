//z6-Merge sort for list
#include<iostream>
#include"mergeSort.h"
using namespace std;
void sort(linkedlist *&head,int left, int right );
void merge(linkedlist*& head,int left, int mid, int right);
void mergesort(linkedlist *&head, int len){
    sort(head, 1,len);
}
void sort(linkedlist *&head,int left, int right ){
    if(right<=left) return ;
    else {
        int mid = (left+right)/2;
        sort(head,left,mid);
        sort(head,mid+1,right);
        merge(head,left,mid,right);
    }
}
void merge(linkedlist*& head,int left, int mid, int right){
    //
    linkedlist* top=new linkedlist;
    top->next=head;
    linkedlist* t=top;//用来找到对应节点
    linkedlist* end=nullptr;;//用来保存尾节点
    linkedlist* begin=top;//用来找到Left的上一个节点
    linkedlist* lptr=nullptr,*mptr=nullptr,*rptr=nullptr;
    for(int i=0;i<=right;i++){
        if(i==left-1) begin=t;
        if(i==left) lptr=t;
        if(i==mid){
            mptr=t;
            rptr=mptr->next;
            t=rptr;
            mptr->next=nullptr;//提前截断，避免出现环的情况
            continue;
        }
        if(i==right){
            end=t->next;
            t->next=nullptr;//同理
            continue;
        }
        t=t->next;
    }
    while(lptr&&rptr){
        if(lptr->data<=rptr->data){
            begin->next=lptr;
            begin=begin->next;
            lptr=lptr->next;
        }else{
            begin->next=rptr;
            begin=begin->next;
            rptr=rptr->next;
        }
    }
    while(lptr){
        begin->next=lptr;
        begin=begin->next;
        lptr=lptr->next;
    }
    while(rptr){
        begin->next=rptr;
        begin=begin->next;
        rptr=rptr->next;
    }
    begin->next=end;
    head=top->next;
    delete top;
}