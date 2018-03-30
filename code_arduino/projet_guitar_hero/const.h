#ifndef HEADER__CONST_H
#define HEADER__CONST_H

#include "Arduino.h"

#define SERIAL_BAUD               9600

// 0 = LOW ; 1 = HIGH
#define BUTTON_PRESSED            0
#define BUTTON_NOT_PRESSED        1


// For LED
#define COLUMN_LEFT               0
#define COLUMN_RIGHT              1

#define MAX_SLOTS                 16
#define MAX_COLUMNS               2
// Where lines are on the LED slots
#define LINE_SLOT                 2

// Game frame refresh in millis
#define GAME_FRAME_REFRESH_MILLIS 200

// States used for player validation (score)
#define STATE_NOTHING_TO_DO       0
#define STATE_HAS_TO_VALIDATE     1
#define STATE_DID_FAIL            2
#define STATE_DID_VALIDATE        3

#endif
