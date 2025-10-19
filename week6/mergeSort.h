#ifndef MERGESORT_H_INCLUDED
#define MERGESORT_H_INCLUDED
struct linkedlist{
    int data;
    linkedlist *next;
};
void mergesort(linkedlist *&head, int len);

#endif // MERGESORT_H_INCLUDED
