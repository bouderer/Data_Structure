//z3-Loops in the Linked List
#include <iostream>
#include "Node.h"
using namespace std;
bool check(node *head) {
    if (!head || !head->next)//不能反了
        return false;
    node *fast = head->next;
    node *slow = head;
    while (fast && slow) {
        if (slow == fast)
            return true;
        slow = slow->next;
        if (fast->next) {//防止报错
                fast = fast->next->next;
        } else
            return false;
    }
    return false;
}