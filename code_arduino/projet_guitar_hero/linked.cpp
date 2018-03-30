#include "linked.h"
#include <Wire.h>

// FIFO

CList::CList( ) {
  first = NULL;
}

void CList::append( CTriangle & triangle ) {;
  if( first == NULL ) {
    first = new SNode();
    first->data = triangle;
    first->next = NULL;
  } else {
    SNode * node = first;

    while( node->next != NULL ) {
      node = node->next;
    }

    node->next = new SNode( );
    node->next->data = triangle;
    node->next->next = NULL;
  }
  
}

SNode * CList::getFirst( ) {
  return first;
}

CTriangle& CList::getTriangleAt( int index ) {
  int iCurrIndex = 0;
  SNode * currNode = first;

  while( iCurrIndex != index && currNode != NULL ) {
    currNode = currNode -> next;
  }

  return currNode->data;
}

void CList::removeFirst( ) {
  if( first != NULL ) {
    SNode * next = first->next;
    delete first;
    first = next;
  }
}

int CList::count() {
  SNode * node = first;
  int i = 0;

  while( node != NULL ) {
    node = node -> next;
    i ++;
  }

  return i;
}
