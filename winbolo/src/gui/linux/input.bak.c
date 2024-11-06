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
#include "../bolo/global.h"
#include "../bolo/backend.h"
#include "input.h"

tankButton tb;
static BYTE scrollKeyCount = 0; /* Used for screen scrolling */
keyItems heldKeys;

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
tankButton inputGetKeys(keyItems *setKeys, bool isMenu) {
  static BYTE gunsightKeyCount = 0; /* Used for gunsight movement */
  Uint8 *keys; /* Keyboard states */
  bool proceed;           /* Ok to proceed */
  proceed = true;

  /* FIXME: Check message window doesn't have focus *
  if (GetForegroundWindow() != hWnd || isMenu == true) {
    return TNONE;
   SDL_PumpEvents();
    SDL_PumpEvents();
 } */
 keys = SDL_GetKeyState(NULL);
 SDL_PumpEvents();
  if (proceed == true) {
    /* Now set up the tank buttons */
    if (keys[(setKeys->kiForward)] == SDL_PRESSED &&  keys[(setKeys->kiRight)] == SDL_PRESSED) {
      tb = TRIGHTACCEL;
    } else if (keys[(setKeys->kiForward)] == SDL_PRESSED && keys[(setKeys->kiLeft)] == SDL_PRESSED) {
      tb = TLEFTACCEL;
    } else if (keys[(setKeys->kiBackward)] == SDL_PRESSED && keys[(setKeys->kiLeft)] == SDL_PRESSED) {
      tb = TLEFTDECEL;
    } else if (keys[(setKeys->kiBackward)] == SDL_PRESSED && keys[(setKeys->kiRight)] == SDL_PRESSED) {
      tb = TRIGHTDECEL;
    } else if (keys[SDLK_q]/* [(setKeys->kiForward)]*/ == SDL_PRESSED) {
      tb = TACCEL;
    } else if (keys[(setKeys->kiBackward)] == SDL_PRESSED) {
      tb = TDECEL;
    } else if (keys[(setKeys->kiLeft)] == SDL_PRESSED) {
      tb = TLEFT;
    } else if (keys[(setKeys->kiRight)] == SDL_PRESSED) {
      tb = TRIGHT;
    } else {
      tb = TNONE;
    }
    /* Get whether the tank is lay a mine */
    if (keys[(setKeys->kiLayMine)] == SDL_PRESSED) {
//      screenTankLayMine();
    }
    scrollKeyCount++;
    if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == false) {
      scrollKeyCount = 0;
      /* Scroll Check */
      if (keys[(setKeys->kiScrollUp)] == SDL_PRESSED) {
//        screenUpdate(up);
      }
      if (keys[(setKeys->kiScrollDown)] == SDL_PRESSED) {
//        screenUpdate(down);
      }
      if (keys[(setKeys->kiScrollLeft)] == SDL_PRESSED) {
//        screenUpdate(left);
      }
      if (keys[(setKeys->kiScrollRight)] == SDL_PRESSED) {
//        screenUpdate(right);
      }
    }
    gunsightKeyCount++;
    if (gunsightKeyCount >= INPUT_GUNSIGHT_WAIT_TIME && isMenu == false) {
      /* Gunsight Check */
      if (keys[(setKeys->kiGunIncrease)] == SDL_PRESSED) {
//        screenGunsightRange(true);
        gunsightKeyCount = 0;
      } else if (keys[(setKeys->kiGunDecrease)] == SDL_PRESSED) {
//        screenGunsightRange(false);
        gunsightKeyCount = 0;
      } else if (gunsightKeyCount > (INPUT_GUNSIGHT_WAIT_TIME + 1)) {
        gunsightKeyCount = INPUT_GUNSIGHT_WAIT_TIME;
      }
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
void inputScroll(keyItems *setKeys, bool isMenu) {
  Uint8 *keys;   /* Keyboard states */
  bool proceed; /* Ok to proceed */

  proceed = true;
  keys = SDL_GetKeyState(NULL);
 SDL_PumpEvents();
  /* Get the tabkButtons here if it is OK to proceed */
  if (proceed == true) {
    scrollKeyCount++;
    if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && isMenu == false) {
      scrollKeyCount = 0;
      /* Scroll Check */
      if (keys[(setKeys->kiScrollUp)] == SDL_PRESSED) {
//        screenUpdate(up);
      }
      if (keys[(setKeys->kiScrollDown)] == SDL_PRESSED) {
 //       screenUpdate(down);
      }
      if (keys[(setKeys->kiScrollLeft)] == SDL_PRESSED) {
//        screenUpdate(left);
      }
      if (keys[(setKeys->kiScrollRight)] == SDL_PRESSED) {
//        screenUpdate(right);
      }
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
bool inputIsFireKeyPressed(keyItems *setKeys, bool isMenu) {
  bool returnValue;       /* Value to return */
  Uint8 *keys;  /* Keyboard states */
  bool proceed;           /* Ok to proceed */

  returnValue = false;
  proceed = true;
  keys = SDL_GetKeyState(NULL);
 SDL_PumpEvents();
 
  /* Get the fire button state here if it is OK to proceed */
  if (proceed == true) {
    if (keys[(setKeys->kiShoot)] == SDL_PRESSED) {
      returnValue = true;
    }
  }
  return returnValue;
}

