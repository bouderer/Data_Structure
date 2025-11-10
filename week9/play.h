#ifndef PLAY_H_INCLUDED
#define PLAY_H_INCLUDED
struct Node {
  Node *lc, *rc;
  char data;
} ;
void query(const Node *root, int &size, int &height);

#endif // PLAY_H_INCLUDED