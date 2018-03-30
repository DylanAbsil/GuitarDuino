#include "triangle.h"


CTriangle:: CTriangle( ) : CTriangle(0) {
}

// 1. Copy Constructor
CTriangle::CTriangle(const CTriangle& other)
{
  iCurrentSlot = other.getSlot();
  iColumn = other.getColumn();
  bDead = other.isDead();
}

// 2. = Operator
CTriangle& CTriangle::operator=(const CTriangle& other)
{
    iCurrentSlot = other.getSlot();
    iColumn = other.getColumn();
    bDead = other.isDead();
    return *this;
}

// 3. Destructor - Useless
CTriangle::~CTriangle()
{
}

CTriangle::CTriangle( int column ) {
  iCurrentSlot = 16;
  iColumn = column;
  bDead = false;
}

void CTriangle::frameUpdate( ) {
  iCurrentSlot --;

  if( iCurrentSlot < 0 ) {
    bDead = true;
  }
}

bool CTriangle:: isTouching( ) {
  return iCurrentSlot == LINE_SLOT;
}

bool CTriangle:: isDead( ) {
  return bDead;
}

int CTriangle:: getColumn( ) {
  return iColumn;
}

int CTriangle:: getSlot( ) {
  return iCurrentSlot;
}
