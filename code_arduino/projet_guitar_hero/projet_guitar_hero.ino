#include <Wire.h>
#include "rgb_lcd.h"
#include "const.h"
#include "game.h"
#include "linked.h"

rgb_lcd lcd;
typedef unsigned char uc;

byte line[8] = {0b00100,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100};
byte triangle[8] = {0b00001,0b00011,0b00111,0b01111,0b01111,0b00111,0b00011,0b00001};
byte triangleLine[8] = {0b00101,0b00111,0b00111,0b01111,0b01111,0b00111,0b00111,0b00101};

CGame * pGame;

int buttonLeft = 12;
int buttonRight = 7;
int buttonValidate = 8;
int led = LED_BUILTIN;
bool bCanPressValidate = true;
bool bGameOver = false;

void setup()
{
  Serial.begin(SERIAL_BAUD);
  lcd.begin(MAX_SLOTS, MAX_COLUMNS);
  lcd.createChar(0, line);
  lcd.createChar(1,triangle);
  lcd.createChar(2,triangleLine);

  pinMode(buttonLeft,INPUT_PULLUP);
  pinMode(buttonValidate, INPUT_PULLUP);
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(led, OUTPUT);

  pGame = new CGame( &lcd );
  pGame->init();
}

void gameEndPrint( char * szText, int code ) {
  if( strlen( szText ) > MAX_SLOTS ) {
    return;
  }
  
  lcd.clear();
  // We center the text
  lcd.setCursor(MAX_SLOTS-strlen(szText)-(strlen(szText)/2), 0);
  lcd.print(szText);
  lcd.setCursor(MAX_SLOTS/2 - 1,1);
  lcd.print(pGame->getScore());

  char scoreToSend[16];
  sprintf(scoreToSend, "%d%d", code, pGame->getScore());
  Serial.print(scoreToSend);
}

void loop()
{

  /*
    Reading Serial, informations of triangles come from an external source
    Here it's a MacOsx program (in Swift) which sends the informations after an audio analysis
      0 : Init/Reset game
      1 : Add Triangle on left column
      2 : Add Triangle on right column
      3 : Add Triangle on both columns
      4 : Song is finished, show a "WIN" + Score
      5 : Game over, we send to the MAC OS X that we lost
      6 : Game won, we send to the MAC OS X that we won
  */
  if( Serial.available() ) {
    int r = Serial.read();

    if( bGameOver == false ) {
       if( r == 1 ) {
        pGame->addTriangle(COLUMN_LEFT);
      } else if( r == 2 ) {
        pGame->addTriangle(COLUMN_RIGHT);
      } else if( r == 3 ) {
        pGame->addTriangle(COLUMN_LEFT);
        pGame->addTriangle(COLUMN_RIGHT);
      } else if( r == 4 ) {
        bGameOver = true;
        gameEndPrint( "GAME WON !", 6 );
        lcd.setRGB(0,255,0);
      }
    } else {
      
    }

    Serial.flush();
  }

  if( bGameOver == true ) {
    return;
  }

  if( digitalRead(buttonValidate) == BUTTON_PRESSED ) {
    if( bCanPressValidate == true && pGame->canValidate() ) {
      bCanPressValidate = false;
      pGame->validate(digitalRead(buttonLeft), digitalRead(buttonRight));
    }
  } else {
    bCanPressValidate = true;
  }

  // If frameUpdate returns true it means GAME OVER.
  if( pGame->frameUpdate() == true ) {
    bGameOver = true;
    gameEndPrint( "GAME OVER", 5 );
    lcd.setRGB(255,0,0);
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
