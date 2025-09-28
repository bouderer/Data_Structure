#ifndef LinkNode_h
#define LinkNode_h
#include <stdlib.h>
struct LinkNode {
    int data;
    LinkNode *next;
	LinkNode(int d, LinkNode *add_on = NULL) {
		data = d;
		next = add_on;
	}
};
typedef LinkNode *LinkList;

#endif
//LinkNode_h