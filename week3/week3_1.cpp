//z3-Delete Duplicate
#include <iostream>
#include "LinkNode.h"
#include <map>
using namespace std;
void delete_duplicate(LinkList &head) {
    if (head == NULL){//头节点为空需要直接返回，否则line 9会报错
        return;
    }
    LinkList t = head->next;
    LinkList prev = head;
    while (t) {
        if (prev->data == t->data) {
            prev->next = t->next;
            LinkList temp=t->next;//用temp记录t指向的下一个节点，否则delete之后line 16会报错
            delete t;
            t = temp;
        } else {
            prev = t;
            t = t->next;
        }
    }
}