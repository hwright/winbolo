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
*Name:          Input
*Filename:      input.c
*Author:        John Morrison
*Creation Date: 16/12/98
*Last Modified:   1/5/00
*Purpose:
*  Keyboard and mouse routines (ie Direct Input routines)
*********************************************************/

#include "SDL.h"
#include "../../bolo/global.h"
#include "../../bolo/backend.h"
#include "input.h"

static tankButton tb;
static BYTE scrollKeyCount = 0; /* Used for screen scrolling */
static keyItems heldKeys;

/*********************************************************
*NAME:          inputSetup
*AUTHOR:        John Morrison
*CREATION DATE: 16/12/98
*LAST MODIFIED: 29/4/00
*PURPOSE:
*  Sets up input systems, direct draw structures etc.
*  Returns whether the operation was successful or not
*
*ARGUMENTS:
* appInst - Handle to the application
* appWnd  - Main Window Handle
*********************************************************/
bool inputSetup() {
  scrollKeyCount = 0;
  /* These are DIK_??? defines */
  heldKeys.kiForward = false;    /* Tank accelerate */
  heldKeys.kiBackward = false;   /* Tank decelerate */
  heldKeys.kiLeft = false;       /* Tank left */
  heldKeys.kiRight = false;      /* Tank right */
  heldKeys.kiShoot = false;      /* Tank shooting */
  heldKeys.kiLayMine = false;     /* Tank lay mine */
  heldKeys.kiGunIncrease = false; /* Increase gunsight length */
  heldKeys.kiGunDecrease = false; /* Decrease gunsight length */
  heldKeys.kiTankView = false;    /* Center on tank */
  heldKeys.kiPillView = false;    /* Pill view */
  heldKeys.kiScrollUp = false;    /* Scroll up */
  heldKeys.kiScrollDown = false;  /* Scroll down */
  heldKeys.kiScrollLeft = false;  /* Scroll left */
  heldKeys.kiScrollRight = false; /* Scroll right */
  return true;
}

/*********************************************************
*NAME:          inputCleanup
*AUTHOR:        John Morrison
*CREATION DATE: 13/12/98
*LAST MODIFIED: 13/12/98
*PURPOSE:
*  Destroys and cleans up input systems, direct draw 
*  structures etc.
*
*ARGUMENTS:
*
*********************************************************/
void inputCleanup(void) {
  return;
}

/*********************************************************
*NAME:          inputGetKeys
*AUTHOR:        John Morrison
*CREATION DATE: 17/12/98
*LAST MODIFIED:   1/5/00
*PURPOSE:
*  Gets the current Buttons that are being pressed.
*  Returns tank buttons being pressed.
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*  isMenu  - True if we are in a menu
*********************************************************/
tankButton inputGetKeys(bool isMenu) {
  static BYTE gunsightKeyCount = 0; /* Used for gunsight movement */

  /* FIXME: Check message window doesn't have focus *
  if (GetForegroundWindow() != hWnd || isMenu == true) {
    return TNONE;
   SDL_PumpEvents();
    SDL_PumpEvents();
 } */
 tb = TNONE;
  /* Now set up the tank buttons */
  if (heldKeys.kiForward == true &&  heldKeys.kiRight == true) {
    tb = TRIGHTACCEL;
  } else if (heldKeys.kiForward == true && heldKeys.kiLeft == true) {
    tb = TLEFTACCEL;
  } else if (heldKeys.kiBackward == true && heldKeys.kiLeft == true) {
    tb = TLEFTDECEL;
  } else if (heldKeys.kiBackward == true && heldKeys.kiRight == true) {
    tb = TRIGHTDECEL;
  } else if (heldKeys.kiForward == true) {
    tb = TACCEL;
  } else if (heldKeys.kiBackward == true) {
    tb = TDECEL;
  } else if (heldKeys.kiLeft == true) {
    tb = TLEFT;
  } else if (heldKeys.kiRight == true) {
    tb = TRIGHT;
  } else {
    tb = TNONE;
  }
  /* Get whether the tank is lay a mine */
  if (heldKeys.kiLayMine == true) {
      screenTankLayMine();
  }
  scrollKeyCount++;
  if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == false) {
    scrollKeyCount = 0;
    /* Scroll Check */
    if (heldKeys.kiScrollUp == true) {
      screenUpdate(up);
    }
    if (heldKeys.kiScrollDown == true) {
      screenUpdate(down);
    }
    if (heldKeys.kiScrollLeft == true) {
      screenUpdate(left);
    }
    if (heldKeys.kiScrollRight == true) {
      screenUpdate(right);
    }
  }
  gunsightKeyCount++;
  if (gunsightKeyCount >= INPUT_GUNSIGHT_WAIT_TIME && isMenu == false) {
    /* Gunsight Check */
    if (heldKeys.kiGunIncrease == true) {
      screenGunsightRange(true);
      gunsightKeyCount = 0;
    } else if (heldKeys.kiGunDecrease == true) {
      screenGunsightRange(false);
      gunsightKeyCount = 0;
    } else if (gunsightKeyCount > (INPUT_GUNSIGHT_WAIT_TIME + 1)) {
      gunsightKeyCount = INPUT_GUNSIGHT_WAIT_TIME;
    }
  }
  return tb;
}

/*********************************************************
*NAME:          inputScroll
*AUTHOR:        John Morrison
*CREATION DATE: 1/5/00
*LAST MODIFIED: 1/5/00
*PURPOSE:
*  Checks and does scrolling of the window
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*  isMenu  - True if we are in a menu
*********************************************************/
void inputScroll(bool isMenu) {
  scrollKeyCount++;
  if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == false) {
    scrollKeyCount = 0;
    /* Scroll Check */
    if (heldKeys.kiScrollUp == true) {
      screenUpdate(up);
    }
    if (heldKeys.kiScrollDown == true) {
      screenUpdate(down);
    }
    if (heldKeys.kiScrollLeft == true) {
      screenUpdate(left);
    }
    if (heldKeys.kiScrollRight == true) {
       screenUpdate(right);
    }
  }
}

/*********************************************************
*NAME:          inputIsFireKeyPressed
*AUTHOR:        John Morrison
*CREATION DATE: 25/12/98
*LAST MODIFIED: 14/11/99
*PURPOSE:
*  Returns whether the fire key is pressed
*  Old Comment dates before hWnd was added:
*      *CREATION DATE: 25/12/98
*      *LAST MODIFIED: 31/12/98
*
*ARGUMENTS:
*     hWnd - The main window
*  setKeys - Structure that holds the key settings
*   isMenu - true if we are in a menu
*********************************************************/
bool inputIsFireKeyPressed(bool isMenu) {
  bool returnValue;       /* Value to return */

  returnValue = false;
 
  /* Get the fire button state here if it is OK to proceed */
  if (heldKeys.kiShoot == true) {
    returnValue = true;
  }
  return returnValue;
}

void inputButtonInput(keyItems *setKeys, int key, bool newState) {
  if ((setKeys->kiForward) == key) {
    heldKeys.kiForward = newState;
  }
  if ((setKeys->kiBackward) == key) {
    heldKeys.kiBackward = newState;
  }
  if ((setKeys->kiLeft) == key) {
    heldKeys.kiLeft = newState;
  }
  if ((setKeys->kiRight) == key) {
    heldKeys.kiRight = newState;
  }
  if ((setKeys->kiShoot) == key) {
    heldKeys.kiShoot = newState;
  }
  if ((setKeys->kiLayMine) == key) {
    heldKeys.kiLayMine = newState;
  }
  if ((setKeys->kiGunIncrease) == key) {
    heldKeys.kiGunIncrease = newState; /* Increase gunsight length */
  }
  if ((setKeys->kiGunDecrease) == key) {
    heldKeys.kiGunDecrease = newState; /* Decrease gunsight length */
  }
  if ((setKeys->kiScrollUp) == key) {
    heldKeys.kiScrollUp = newState;    /* Scroll up */
  }
  if ((setKeys->kiScrollDown) == key) {
    heldKeys.kiScrollDown = newState;  /* Scroll down */
  }
  if ((setKeys->kiScrollLeft) == key) {
    heldKeys.kiScrollLeft = newState;  /* Scroll left */
  }
  if ((setKeys->kiScrollRight) == key) {
    heldKeys.kiScrollRight = newState; /* Scroll right */
  }
}


