#ifndef HEADER__GAME_I
#define HEADER__GAME_I

#include <Wire.h>
#include "rgb_lcd.h"
#include "const.h"
#include "triangle.h"
#include "linked.h"

#define NULL 0

class CGame {
  private:
    // About Triangles (Entities)
    CList triangles;
    int iCurrTrianglesNum;
    /*
      Even if i can count triangles with CList functions it's faster if i count them myself
      Otherwise i would have to browse all the CList everytime which may affect performances
    */

    rgb_lcd * lcd;

    /*
      :: Validate State refers to the score, if the player failed or not.
      You can see the constants used for it in const.h

      :: Can Validate refers to the "Validate" button used to play
      If the player already validated for a given game frame, he can't validate again
      He'll have to wait for the next frame to push the button again
    */
    bool bCanValidate;
    int iValidateState;


    // iLastFrameMilliseconds represents last time game frame was refreshed.
    int iLastFrameMilliseconds;

    /*
      :: iScore refers to the player score
      :: iBad refers to the negative score the player accumulate, if it
         get higher than a certain amount, it will print game over
    */
    int iScore;
    int iBad;
  public:
    CGame( rgb_lcd * lcd );

    void init();
    bool frameUpdate( );
    void clearLcd(bool printScore);

    void addTriangle(int column);
    void moveTriangles();

    bool canValidate();
    void validate(int left, int right);
    int getScore();
};

#endif
