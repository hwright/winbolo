/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


/*********************************************************
*Name:          scroll
*Filename:      scroll.c
*Author:        John Morrison
*Creation Date: 16/01/99
*Last Modified: 10/06/01
*Purpose:
*  Handles scrolling on the screen. Auto scrolling and
*  keeping the object in the centre of the screen
*********************************************************/


#include "global.h"
#include "backend.h"
#include "screen.h"
#include "scroll.h"

/* Is it auto scrolling or not */
static bool autoScroll = false;
static BYTE scrollX = 0;
static BYTE scrollY = 0;
static BYTE xPositive = true;
static BYTE yPositive = true;
static bool autoScrollOverRide = false;
static bool mods = false;


/*********************************************************
*NAME:          scrollSetScrollType
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Modifies state of autoscrolling
*
*ARGUMENTS:
*  isAuto - Set to on or off?
*********************************************************/
void scrollSetScrollType(bool isAuto) {
  autoScroll = isAuto;
  autoScrollOverRide = false;
  if (isAuto == false) {
    scrollX = 0;
    scrollY = 0;
    mods = false;
  } 
}

/*********************************************************
*NAME:          scrollCenterObject
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Centres the screen on the object
*
*ARGUMENTS:
*  xValue - Pointer to hold new X co-ordinate
*  yValue - Pointer to hold new Y co-ordinate
*  objectX - Object to centre on X co-ordinate 
*  objectY - Object to centre on Y co-ordinate 
*********************************************************/
void scrollCenterObject(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY) {
  *xValue = objectX - SCROLL_CENTER;
  *yValue = objectY - SCROLL_CENTER;
  autoScrollOverRide = false;
}

/*********************************************************
*NAME:          scrollUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Called every game tick. Checks to see if the screen 
*  is required to be moved because the tank has moved
*  etc.
*  If the object is not a tank the last 3 parameters
*  are ignored. It returns if a recalculation is needed
*
*ARGUMENTS:
*  pb        - Pointer to the pillboxes structure
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  isTank    - Is the object a tank
*  gunsightX - The gunsights X position
*  gunsightY - The gunishgts Y position
*  speed     - The speed of the tank
*  armour    - Amount of armour on the tank
*  angle     - Tank travelling angle
*  manual    - Is it manual move (ie by keys, not tank)
*********************************************************/
bool scrollUpdate(pillboxes *pb, BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, bool isTank, BYTE gunsightX, BYTE gunsightY, BYTE speed, BYTE armour, TURNTYPE angle, bool manual) {
  bool returnValue; /* Value to return */
  
  returnValue = true;
  if (screenTankIsDead() == true) {
    returnValue = false;
  } else if (manual == true) {
    returnValue = scrollManual(xValue, yValue, objectX, objectY, angle);
  } else if (autoScroll == true && isTank == true && armour <= TANK_FULL_ARMOUR && autoScrollOverRide == false) {
    /* Calculate using the autoscroll function */
    returnValue = scrollAutoScroll(pb, xValue, yValue, objectX, objectY, gunsightX, gunsightY, speed, angle);
  } else {
   /* calculate not using auto scroll functions */
    returnValue = scrollNoAutoScroll(xValue, yValue, objectX, objectY, angle);
  }

  return returnValue;

}

/*********************************************************
*NAME:          scrollCheck
*AUTHOR:        John Morrison
*CREATION DATE: 19/11/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Returns whether an item is on screen or not
*
*ARGUMENTS:
*  xValue    - Current X co-ordinate
*  yValue    - Current Y co-ordinate
*  objectX   - Objects X co-ordinate 
*  objectY   - Objects Y co-ordinate 
*********************************************************/
bool scrollCheck(BYTE xValue, BYTE yValue, BYTE objectX, BYTE objectY) {
  bool returnValue; /* Used internally */
    
  returnValue = true;
  /* Check to see if moving towards screen edge */
  if ((objectX - xValue) > MAIN_SCREEN_SIZE_X) {
    xValue++;
    returnValue = false;
  }
  if ((objectX-1) < xValue) {
    xValue--;
    returnValue = false;
  }
  if ((objectY - yValue) > MAIN_SCREEN_SIZE_Y) {
    returnValue = false;
  }
  if (objectY <= yValue) {
   returnValue = false;
  }

  return returnValue;
}


/*********************************************************
*NAME:          scrollManual
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 10/06/01
*PURPOSE:
*  Movement scroll keys have been pressed. Returns if a 
*  movement occurs
*
*ARGUMENTS:
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Objects X co-ordinate 
*  objectY   - Objects Y co-ordinate 
*  angle     - Items Angle
*********************************************************/
bool scrollManual(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, TURNTYPE angle) {
  bool returnValue; /* Used internally */
  bool leftPos;
  bool rightPos;
  bool upPos;
  bool downPos;

  leftPos = false;
  rightPos = false;
  upPos = false;
  downPos = false;
  returnValue = false;
  autoScrollOverRide = true;

  if (angle >= BRADIANS_SSWEST && angle <= BRADIANS_NNWEST) {
    rightPos = true;
  }
  if (angle >= BRADIANS_NNEAST && angle <= BRADIANS_SSEAST) {
    leftPos = true;
  }
  if (angle >= BRADIANS_SEASTE  && angle <= BRADIANS_SWESTW) {
    downPos = true;
  }
  if (angle <= BRADIANS_NEASTE || angle >= BRADIANS_NWESTW) {
    upPos = true;
  }

  
  /* Check to see if moving towards screen edge */
  if ((objectX - (*xValue)) > MAIN_SCREEN_SIZE_X && leftPos == true) {
    (*xValue)++;
    returnValue = true;
  }
  if ((objectX-2) < (*xValue) && rightPos == false) {
    (*xValue)--;
    returnValue = true;
  }
  if ((objectY - (*yValue)) > MAIN_SCREEN_SIZE_Y && downPos == true) {
    (*yValue)++;
    returnValue = true;
  }
  if (objectY <= (*yValue) && upPos == true) {
   (*yValue)--;
   returnValue = true;
  }
  if (mods == true && returnValue == true) {
    mods = false;
  }

  return returnValue;
}

/*********************************************************
*NAME:          scrollNoAutoScroll
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 19/11/99
*PURPOSE:
*  Checks to see if the screen is required to be moved 
*  because the object is moving off screen. Doesn't 
*  use autoscrolling features. Returns if a recalculation
*  of the screen is needed
*
*ARGUMENTS:
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  angle     - Turntype angle
*********************************************************/
bool scrollNoAutoScroll(BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, TURNTYPE angle) {
  bool returnValue; /* Value to return */
  bool leftPos;
  bool rightPos;
  bool upPos;
  bool downPos;

  leftPos = false;
  rightPos = false;
  upPos = false;
  downPos = false;
  returnValue = false;

  if (angle >= BRADIANS_SSWEST && angle <= BRADIANS_NNWEST) {
    rightPos = true;
  }
  if (angle >= BRADIANS_NNEAST && angle <= BRADIANS_SSEAST) {
    leftPos = true;
  }
  if (angle >= BRADIANS_SEASTE  && angle <= BRADIANS_SWESTW) {
    downPos = true;
  }
  if (angle <= BRADIANS_NEASTE || angle >= BRADIANS_NWESTW) {
    upPos = true;
  }

  /* Check to see if moving towards screen edge */
  if ((objectX - (*xValue)) >= (MAIN_SCREEN_SIZE_X-NO_SCROLL_EDGE) && leftPos == true) {
    (*xValue)++;
    returnValue = true;
  }
  if ((objectX-1) < (*xValue)+NO_SCROLL_EDGE && rightPos == true) {
    (*xValue)--;
    returnValue = true;
  }
  if ((objectY - (*yValue)) >= (MAIN_SCREEN_SIZE_Y-NO_SCROLL_EDGE) && downPos == true) {
    (*yValue)++;
    returnValue = true;
  }
  if (objectY <= (*yValue)+NO_SCROLL_EDGE && upPos == true) {
   (*yValue)--;
   returnValue = true;
  }
  if (mods == true && returnValue == true) {
    mods = false;
  }
  if (returnValue == true) {
    autoScrollOverRide = false;
  }

  return returnValue;
}

/*********************************************************
*NAME:          scrollAutoScroll
*AUTHOR:        John Morrison
*CREATION DATE: 16/1/99
*LAST MODIFIED: 16/1/99
*PURPOSE:
*  Checks to see if the screen is required to be moved 
*  because the object is moving off screen. Uses the
*  autoscrolling features. Returns if a recalculation
*  of the screen is needed
*
*ARGUMENTS:
*  pb        - Pointer to the pillboxes structure
*  xValue    - Pointer to hold new X co-ordinate
*  yValue    - Pointer to hold new Y co-ordinate
*  objectX   - Object to centre on X co-ordinate 
*  objectY   - Object to centre on Y co-ordinate 
*  gunsightX - The gunsights X position
*  gunsightY - The gunishgts Y position
*  speed     - The speed of the tank
*  angle     - Angle of the tank
*********************************************************/
bool scrollAutoScroll(pillboxes *pb, BYTE *xValue, BYTE *yValue, BYTE objectX, BYTE objectY, BYTE gunsightX, BYTE gunsightY, BYTE speed, TURNTYPE angle) {
  bool returnValue;        /* Value to return */

  returnValue = false;


  if (scrollX == 0 && scrollY == 0) {
    /* Check to see if moving towards screen edge */
    if (((gunsightX-1) - (*xValue)) >= (MAIN_SCREEN_SIZE_X)) {
      scrollX = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollX == 0) {
        scrollX = 1;
      }
      xPositive = true;
      mods = true;
    }
    if ((gunsightX) < (*xValue)) {
      scrollX = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollX == 0) {
        scrollX = 1;
      }
      xPositive = false;
      mods = true;
    }
    if (((gunsightY-1)- (*yValue)) >= (MAIN_SCREEN_SIZE_Y)) {
      scrollY = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollY == 0) {
        scrollY = 1;
      }

      yPositive = true;
      mods = true;
    }
    if (gunsightY < (*yValue)) {
      scrollY = (BYTE) (speed / SCROLL_DIVIDE);
      if (scrollY == 0) {
        scrollY = 1;
      }

      yPositive = false;
      mods = true;
    } 
  }


  if (scrollX > 0) {
    returnValue = true;
    scrollX--;
    if (xPositive == true) {
      (*xValue)++;
    } else {
      (*xValue)--;
    }
  }
  if (scrollY > 0) {
    scrollY--;
    returnValue = true;
    if (yPositive == true) {
      (*yValue)++;
    } else {
      (*yValue)--;
    }
  }
  if (scrollX == 0 && scrollY == 0) {
    mods = false;
  }

  return returnValue;
}
