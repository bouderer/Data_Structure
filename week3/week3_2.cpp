//z3-Insert for single link list with head node
#include<iostream>
#include"ListNode.h"
using namespace std;
void List::insert(int toadd, int pos){
    ListNode* t=head;//指向头节点
    for(int i =1;i<pos;i++){
        t=t->next;
    }
    ListNode* next=t->next;
    ListNode* cur=new ListNode;
    cur->data=toadd;
    t->next=cur;
    cur->next=next;

}