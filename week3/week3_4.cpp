//z3-Remove for single link list with head node
#include<iostream>
#include"List_Node.h"
using namespace std;
void List::remove(int pos){
    ListNode* t=head;
    for (int i=1;i<pos;i++){
        t=t->next;
    }
    ListNode* cur=t->next;
    ListNode* next=cur->next;
    t->next = next;
    delete cur;

}