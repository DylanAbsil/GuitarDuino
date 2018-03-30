#include "game.h"
#include "const.h"

// Constructor, we need the addr of the LCD object to print triangles & lines.
CGame::CGame( rgb_lcd * lcd ) {
  this->lcd = lcd;
}

// CGame::init is called when you need to init or restart the game
void CGame::init(){
  this->triangles = CList();

  iCurrTrianglesNum = 0;
  iLastFrameMilliseconds = 0;
  iScore = 0;
  iBad = 0;
  bCanValidate = true;
  iValidateState = STATE_NOTHING_TO_DO;
}

// CGame::addTriangle adds a triangle to the game, it also checks if you try to add too much triangles
void CGame::addTriangle(int column) {
  if( iCurrTrianglesNum < MAX_SLOTS * MAX_COLUMNS ) {
    CTriangle tr(column);
    triangles.append( tr );
    iCurrTrianglesNum ++;
  }
}

/*
 * CGame::validate is called when the player pressed the "Validate" button.
 * This function defines if player validated his point or failed.
 * The result will be used for the next Game Frame, before cleaning & refreshing everything
 */
void CGame::validate(int left, int right) {
  /*
    left = Left Button value
    right = Right Button Value

    Since we use INPUT_PULLUP for the buttons :
      - HIGH (1) means Button not pressed
      - LOW (0) means Button pressed

    Anyway there is a def for Button PRESSED : BUTTON_PRESSED
  */
  if( iValidateState == STATE_NOTHING_TO_DO || (left == BUTTON_NOT_PRESSED && right == BUTTON_NOT_PRESSED)) {
    iValidateState = STATE_DID_FAIL;
    return;
  }

  bCanValidate = false;
  bool bMustHaveLeft = false, bMustHaveRight = false;
  bool bLeft = (left == BUTTON_PRESSED), bRight = (right == BUTTON_PRESSED);

  SNode * node = triangles.getFirst( );

  // Find which one of the columns have to be validated, both or only one ?
  while( node != NULL ) {
    if(node->data.getSlot()==LINE_SLOT) {
      if(node->data.getColumn()==COLUMN_LEFT){
        bMustHaveLeft = true;
      } else {
        bMustHaveRight = true;
      }
    }

    node = node->next;
  }
  if( bMustHaveLeft == bLeft && bMustHaveRight == bRight ) {
    iValidateState = STATE_DID_VALIDATE;
  }
}

// CGame::canValidate allows the player to push the "Validate Button" if it returns true
bool CGame::canValidate() {
  return bCanValidate;
}

// CGame::clearLcd cleans everything and put the lines + the score
void CGame::clearLcd(bool printScore) {
  lcd->clear();
  lcd->setCursor(LINE_SLOT, COLUMN_LEFT);
  lcd->write((unsigned char)0);
  lcd->setCursor(LINE_SLOT, COLUMN_RIGHT);
  lcd->write((unsigned char)0);

  if( printScore == true ) {
    lcd->setCursor(0,COLUMN_RIGHT);
    lcd->print(iScore);
  }
}

/*
  CGame::moveTriangles resets the ValidateState for player score
  It's also moving all triangles forward and killing triangles going pass the LCD screen
  It removes them from the LIST (list which is a FIFO)
  Afterwards, game is looking for triangles to validate (if they are on the line)
  If there aren't any triangles on line, it will stay at STATE_NOTHING_TO_DO
  Then, we'll be able later to detect if player did an action when he had to do nothing
  OR if he didn't do anything or failed when he has to validate.
*/
void CGame::moveTriangles() {
  iValidateState = STATE_NOTHING_TO_DO;

  SNode * node = triangles.getFirst( );

  while( node != NULL ) {
    node->data.frameUpdate();

    if( node->data.isDead() ) {
      iCurrTrianglesNum --;
      triangles.removeFirst();
      node = triangles.getFirst();
    } else {
      lcd->setCursor(node->data.getSlot(),node->data.getColumn());
      if(node->data.isTouching()==true){
        lcd->write((unsigned char)2);
      } else {
        lcd->write((unsigned char)1);
      }

      if( node->data.getSlot()==LINE_SLOT) {
        iValidateState = STATE_HAS_TO_VALIDATE;
      }

      node = node->next;
    }
  }
}

// CGame::getScore() returns the score, used for "GAME OVER"
int CGame::getScore() {
  return iScore;
}

/*
  CGame::frameUpdate( ) return TRUE if game is OVER, otherwise FALSE

  The game frame is updated each GAME_FRAME_REFRESH_MILLIS

  Before cleaning the LCD, moving triangles and printing everything to the LCD screen
    --> It's counting the points based on two variables :
        * iScore is the total score for the game
        * iBad is the total negative points the player accumulated, it's also used to
          print the red color on the screen, if it's going full red means RGB : 255,0,0
          then the game is OVER and the function returns TRUE so the program can print the "GAME OVER"

  After the score has been updated, the game frame cleans the actual display, refresh the triangles positions
  and print everything again.
  It also resets the bCanValidate variable which permits the player to press the "Validate" button again
*/
bool CGame::frameUpdate( ) {
  int currMillis = millis();

  if( currMillis - iLastFrameMilliseconds < GAME_FRAME_REFRESH_MILLIS ) {
    return false;
  }

  iLastFrameMilliseconds = currMillis;

  if( iValidateState == STATE_HAS_TO_VALIDATE || iValidateState == STATE_DID_FAIL ) {
    iBad ++;
  } else if( iValidateState == STATE_DID_VALIDATE ) {
    iScore ++;
    iBad -= 2;
  }

  if( iBad < 0 ) {
    iBad = 0;
  }

  int iR = min((4*iBad), 255);

  lcd->setRGB(255,255-iR, 255-iR);

  if( iR == 255 ) {
    return true;
  }

  clearLcd(true);
  moveTriangles();

  bCanValidate = true;
}
