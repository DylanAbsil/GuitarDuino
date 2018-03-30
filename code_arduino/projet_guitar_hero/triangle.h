#ifndef HEADER__TRIANGLES_I
#define HEADER__TRIANGLES_I

#include "const.h"

class CTriangle {
  private:
    int iCurrentSlot;
    int iColumn;
    bool bDead;
   public:
    CTriangle();
    CTriangle(int column);
    CTriangle(const CTriangle& other);
    ~CTriangle();

    int getSlot( );
    int getColumn();
    bool isDead();
    void frameUpdate( );
    bool isTouching( );

    CTriangle& operator=(const CTriangle& other);
};






#endif
