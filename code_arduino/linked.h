#ifndef HEADER__LINKED_I
#define HEADER__LINKED_I

#include "const.h"
#include "triangle.h"

#define NULL 0

typedef struct Node {
  CTriangle data;
  struct Node * next;
} SNode;

class CList {
private:
  SNode * first;
public:
  CList();
  void append( CTriangle & triangle );
  void removeFirst( );
  SNode * getFirst();
  CTriangle& getTriangleAt( int index );
  int count();
};

#endif
