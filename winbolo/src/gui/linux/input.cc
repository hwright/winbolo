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

#include "input.h"

#include <mutex>
#include <shared_mutex>

#include "../../bolo/backend.h"
#include "../../bolo/global.h"
#include "SDL.h"

static tankButton tb;
static BYTE scrollKeyCount = 0; /* Used for screen scrolling */
static keyItems heldKeys;
static std::shared_mutex keysMutex;

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
  std::unique_lock l(keysMutex);
  scrollKeyCount = 0;
  /* These are DIK_??? defines */
  heldKeys.kiForward = false;     /* Tank accelerate */
  heldKeys.kiBackward = false;    /* Tank decelerate */
  heldKeys.kiLeft = false;        /* Tank left */
  heldKeys.kiRight = false;       /* Tank right */
  heldKeys.kiShoot = false;       /* Tank shooting */
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
void inputCleanup(void) { return; }

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
  std::shared_lock l(keysMutex);

  /* FIXME: Check message window doesn't have focus *
  if (GetForegroundWindow() != hWnd || isMenu == true) {
    return TNONE;
   SDL_PumpEvents();
    SDL_PumpEvents();
 } */
  tb = TNONE;
  /* Now set up the tank buttons */
  if (static_cast<bool>(heldKeys.kiForward) &&
      static_cast<bool>(heldKeys.kiRight)) {
    tb = TRIGHTACCEL;
  } else if (static_cast<bool>(heldKeys.kiForward) &&
             static_cast<bool>(heldKeys.kiLeft)) {
    tb = TLEFTACCEL;
  } else if (static_cast<bool>(heldKeys.kiBackward) &&
             static_cast<bool>(heldKeys.kiLeft)) {
    tb = TLEFTDECEL;
  } else if (static_cast<bool>(heldKeys.kiBackward) &&
             static_cast<bool>(heldKeys.kiRight)) {
    tb = TRIGHTDECEL;
  } else if (static_cast<bool>(heldKeys.kiForward)) {
    tb = TACCEL;
  } else if (static_cast<bool>(heldKeys.kiBackward)) {
    tb = TDECEL;
  } else if (static_cast<bool>(heldKeys.kiLeft)) {
    tb = TLEFT;
  } else if (static_cast<bool>(heldKeys.kiRight)) {
    tb = TRIGHT;
  } else {
    tb = TNONE;
  }
  /* Get whether the tank is lay a mine */
  if (static_cast<bool>(heldKeys.kiLayMine)) {
    screenTankLayMine();
  }
  scrollKeyCount++;
  if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && !isMenu) {
    scrollKeyCount = 0;
    /* Scroll Check */
    if (static_cast<bool>(heldKeys.kiScrollUp)) {
      screenUpdate(up);
    }
    if (static_cast<bool>(heldKeys.kiScrollDown)) {
      screenUpdate(down);
    }
    if (static_cast<bool>(heldKeys.kiScrollLeft)) {
      screenUpdate(left);
    }
    if (static_cast<bool>(heldKeys.kiScrollRight)) {
      screenUpdate(right);
    }
  }
  gunsightKeyCount++;
  if (gunsightKeyCount >= INPUT_GUNSIGHT_WAIT_TIME && !isMenu) {
    /* Gunsight Check */
    if (static_cast<bool>(heldKeys.kiGunIncrease)) {
      screenGunsightRange(true);
      gunsightKeyCount = 0;
    } else if (static_cast<bool>(heldKeys.kiGunDecrease)) {
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
  std::shared_lock l(keysMutex);
  scrollKeyCount++;
  if (scrollKeyCount >= INPUT_SCROLL_WAIT_TIME && !isMenu) {
    scrollKeyCount = 0;
    /* Scroll Check */
    if (static_cast<bool>(heldKeys.kiScrollUp)) {
      screenUpdate(up);
    }
    if (static_cast<bool>(heldKeys.kiScrollDown)) {
      screenUpdate(down);
    }
    if (static_cast<bool>(heldKeys.kiScrollLeft)) {
      screenUpdate(left);
    }
    if (static_cast<bool>(heldKeys.kiScrollRight)) {
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
  std::shared_lock l(keysMutex);
  bool returnValue; /* Value to return */

  returnValue = false;

  /* Get the fire button state here if it is OK to proceed */
  if (static_cast<bool>(heldKeys.kiShoot)) {
    returnValue = true;
  }
  return returnValue;
}

void inputButtonInput(keyItems *setKeys, int key, bool newState) {
  std::unique_lock l(keysMutex);
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
    heldKeys.kiScrollUp = newState; /* Scroll up */
  }
  if ((setKeys->kiScrollDown) == key) {
    heldKeys.kiScrollDown = newState; /* Scroll down */
  }
  if ((setKeys->kiScrollLeft) == key) {
    heldKeys.kiScrollLeft = newState; /* Scroll left */
  }
  if ((setKeys->kiScrollRight) == key) {
    heldKeys.kiScrollRight = newState; /* Scroll right */
  }
}
