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

#include "backend.h"

#include <stdio.h>

#include "../server/servercore.h"
#include "client.h"
#include "global.h"
#include "lgm.h"
#include "messages.h"
#include "screen.h"
#include "types.h"

#ifdef _WIN32

#else
#include <sys/time.h>
#include <sys/types.h>

static struct timeval start_time;
#endif

bool threadsGetContext();
void serverCoreCenterTank();
void clientSoundDist(sndEffects value, BYTE mx, BYTE my);
void serverCoreSoundDist(sndEffects value, BYTE mx, BYTE my);

/* Current building item selected */
static buildSelect BsCurrent = BsTrees;

/*********************************************************
 *NAME:          screenGetNumNeutralBases
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/2/99
 *LAST MODIFIED: 21/2/99
 *PURPOSE:
 * Returns the number of neutral bases
 *
 *ARGUMENTS:
 *
 *********************************************************/
BYTE screenGetNumNeutralBases(void) {
  if (!threadsGetContext()) {
    return clientGetNumNeutralBases();
  }
  return serverCoreGetNumNeutralBases();
}

/*********************************************************
 *NAME:          screenMapIsMine
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/8/99
 *LAST MODIFIED: 31/8/99
 *PURPOSE:
 * Returns whether a mines exists at a map square
 *
 *ARGUMENTS:
 *  bmx - Map X position
 *  bmy - Map X position
 *********************************************************/
bool screenMapIsMine(BYTE bmx, BYTE bmy) {
  if (!threadsGetContext()) {
    return clientMapIsMine(bmx, bmy);
  }
  return serverCoreMapIsMine(bmx, bmy);
}

/*********************************************************
 *NAME:          screenGetPlayers
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/01/99
 *LAST MODIFIED: 05/05/01
 *PURPOSE:
 * Returns the players object
 *
 *ARGUMENTS:
 *
 *********************************************************/
players *screenGetPlayers() {
  if (threadsGetContext() || clientGetPlayers() == nullptr) {
    return serverCoreGetPlayers();
  }
  return clientGetPlayers();
}

/*********************************************************
 *NAME:          screenBasesMigrate
 *AUTHOR:        Minhiriath
 *CREATION DATE: 19/01/99
 *LAST MODIFIED: 05/05/01
 *PURPOSE:
 * runs the correct bases migrate functions
 *
 *ARGUMENTS:
 *
 *********************************************************/
void screenBasesMigrate(BYTE playerNumOldOwner, BYTE playerNumNewOwner) {
  if (threadsGetContext()) {
    serverCoreBasesMigrate(playerNumOldOwner, playerNumNewOwner);
  }
  clientBasesMigrate(playerNumOldOwner, playerNumNewOwner);
}

/*********************************************************
 *NAME:          screenPillsMigratePlanted
 *AUTHOR:        Minhiriath
 *CREATION DATE: 15/03/2009
 *LAST MODIFIED: 15/03/2009
 *PURPOSE:
 *  Migrates planted pills
 *
 *ARGUMENTS:
 * playerNumOldOwner - old owner
 * playerNumNewOwner - new owner
 *********************************************************/
void screenPillsMigratePlanted(BYTE playerNumOldOwner, BYTE playerNumNewOwner) {
  if (threadsGetContext()) {
    serverCorePillsMigratePlanted(playerNumOldOwner, playerNumNewOwner);
  }
  clientPillsMigratePlanted(playerNumOldOwner, playerNumNewOwner);
}

/*********************************************************
 *NAME:          screenGetTankFromPlayer
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/00
 *LAST MODIFIED: 21/10/00
 *PURPOSE:
 * Returns the tank for a specific player num. In a client
 * it always return our own tank
 *
 *ARGUMENTS:
 *  playerNum - Tank to get for this player number
 *********************************************************/
tank *screenGetTankFromPlayer(BYTE playerNum) {
  if (!threadsGetContext()) {
    return clientGetTankFromPlayer(playerNum);
  }
  return serverCoreGetTankFromPlayer(playerNum);
}

/*********************************************************
 *NAME:          screenGetTankPlayer
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/10/99
 *LAST MODIFIED: 31/10/99
 *PURPOSE:
 * Returns this tanks player number
 *
 *ARGUMENTS:
 *  value - Pointer to the tank
 *********************************************************/
BYTE screenGetTankPlayer(tank *value) {
  if (!threadsGetContext()) {
    return clientGetTankPlayer(value);
  }
  return serverCoreGetTankPlayer(value);
}

/*********************************************************
 *NAME:          screenNumBases
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 21/12/98
 *PURPOSE:
 *  Returns the number of bases
 *
 *ARGUMENTS:
 *
 *********************************************************/
BYTE screenNumBases(void) {
  if (!threadsGetContext()) {
    return clientNumBases();
  }
  return serverCoreNumBases();
}

/*********************************************************
 *NAME:          screenNumPills
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 21/12/98
 *PURPOSE:
 *  Returns the number of pillboxes
 *
 *ARGUMENTS:
 *
 *********************************************************/
BYTE screenNumPills(void) {
  if (!threadsGetContext()) {
    return clientNumPills();
  }
  return serverCoreNumPills();
}

/*********************************************************
 *NAME:          screenGetLgmFromPlayerNum
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/9/00
 *LAST MODIFIED: 23/9/00
 *PURPOSE:
 * Returns the lgm structure for a particular player number.
 * Always your own for a client.
 *
 *ARGUMENTS:
 *  playerNum - the player numbers lgm to get
 *********************************************************/
lgm *screenGetLgmFromPlayerNum(BYTE playerNum) {
  if (!threadsGetContext()) {
    return clientGetLgmFromPlayerNum(playerNum);
  }
  return serverCoreGetLgmFromPlayerNum(playerNum);
}

/*********************************************************
 *NAME:          screenGetTankWorld
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/12/00
 *LAST MODIFIED: 28/12/00
 *PURPOSE:
 * Gets the tank world co-ordinates from a lgman object
 * returns own tank co-ordinates in a client game
 *
 *ARGUMENTS:
 *  lgmans - Pointer to the lgm structure
 *  x      - Pointer to hold World X Co-ordinates
 *  y      - Pointer to hold World Y Co-ordinates
 *********************************************************/
void screenGetTankWorldFromLgm(lgm *lgmans, WORLD *x, WORLD *y) {
  if (!threadsGetContext()) {
    clientGetTankWorldFromLgm(lgmans, x, y);
  } else {
    serverCoreGetTankWorldFromLgm(lgmans, x, y);
  }
}

/*********************************************************
 *NAME:          screenGetRandStart
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/99
 *LAST MODIFIED: 18/1/99
 *PURPOSE:
 * Places a random start position into the parmeters passed
 *
 *ARGUMENTS:
 *  mx - Pointer to hold Map X Co-ordinates
 *  my - Pointer to hold Map Y Co-ordinates
 *  dir - Direction facing
 *********************************************************/
void screenGetRandStart(BYTE *mx, BYTE *my, TURNTYPE *dir) {
  if (!threadsGetContext()) {
    clientGetRandStart(mx, my, dir);
  } else {
    serverCoreGetRandStart(mx, my, dir);
  }
}

/*********************************************************
 *NAME:          screenCheckTankRange
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/04/01
 *LAST MODIFIED: 11/04/01
 *PURPOSE:
 * Checks to see if there is enemy tank within range
 * supplied. Returns true if we are in the clear
 *
 *ARGUMENTS:
 *  xValue    - X Value
 *  yValue    - Y Value
 *  playerNum - Player number to check against
 *  distance  - Distance to be less then to return false
 *********************************************************/
bool screenCheckTankRange(BYTE x, BYTE y, BYTE playerNum, double distance) {
  if (!threadsGetContext()) {
    return clientCheckTankRange(x, y, playerNum, distance);
  }
  return serverCoreCheckTankRange(x, y, playerNum, distance);
}

/*********************************************************
 *NAME:          screenCheckPillsRange
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/04/01
 *LAST MODIFIED: 9/04/01
 *PURPOSE:
 * Checks to see if there is enemy pill within range
 * supplied. Returns true if we are in the clear
 *
 *ARGUMENTS:
 *  xValue    - X Value
 *  yValue    - Y Value
 *  playerNum - Player number to check against
 *  distance  - Distance to be less then to return true
 *********************************************************/
bool screenCheckPillsRange(BYTE xValue, BYTE yValue, BYTE playerNum,
                           double distance) {
  if (!threadsGetContext()) {
    return clientCheckPillsRange(xValue, yValue, playerNum, distance);
  }
  return serverCoreCheckPillsRange(xValue, yValue, playerNum, distance);
}

/*********************************************************
 *NAME:          serverCoreServerConsoleMessage
 *AUTHOR:        John Morrison
 *CREATION DATE: 8/7/00
 *LAST MODIFIED: 8/7/00
 *PURPOSE:
 * Displays a server message on the console
 *
 *ARGUMENTS:
 *  mode - Server mode
 ********************************************************/
void screenServerConsoleMessage(const char *msg) {
  if (threadsGetContext()) {
    serverCoreServerConsoleMessage(msg);
  }
}

/*********************************************************
 *NAME:          clientMessageAdd
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/1/99
 *LAST MODIFIED:  4/7/00
 *PURPOSE:
 *  Functions call this to display a message. They must
 *  pass the message type so that it can be determined
 *  whether the header should be printed etc.
 *
 *ARGUMENTS:
 *  msgType - The type of the message
 *  top     - The message to print in the top line
 *  bottom  - The message to print in the bottom line
 *********************************************************/
void messageAdd(messageType msgType, const char *top, const char *bottom) {
  if (!threadsGetContext()) {
    clientMessageAdd(msgType, top, bottom);
  } else {
    serverMessageAdd(msgType, top, bottom);
  }
}

/*********************************************************
 *NAME:          screenTankInView
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/8/99
 *LAST MODIFIED:  8/9/00
 *PURPOSE:
 * Returns if the item at checkX, checkY is in viewing
 * range of playre playerNum and any of its pillboxes
 *
 *ARGUMENTS:
 * playerNum - PlayerNum to check
 * checkX    - X Position to check
 * checkY    - Y Position to check
 *********************************************************/
bool screenTankInView(BYTE playerNum, BYTE checkX, BYTE checkY) {
  if (!threadsGetContext()) {
    return clientTankInView(playerNum, checkX, checkY);
  } else {
    return serverCoreTankInView(playerNum, checkX, checkY);
  }
}

/*********************************************************
 *NAME:          screenCenterTank
 *AUTHOR:        John Morrison
 *CREATION DATE: 3/1/99
 *LAST MODIFIED: 3/1/99
 *PURPOSE:
 *  Centers the screen around the tank.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void screenCenterTank() {
  if (!threadsGetContext()) {
    clientCenterTank();
  } else {
    serverCoreCenterTank();
  }
}

/*********************************************************
 *NAME:          soundDist
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/01/99
 *LAST MODIFIED: 05/05/01
 *PURPOSE:
 *  Calculates whether a soft sound of a loud sound should
 *  be played and passes paremeters to frontend
 *
 *ARGUMENTS:
 *  value - Sound effect to be played
 *  mx    - Map X co-ordinatate for the sound origin
 *  my    - Map Y co-ordinatate for the sound origin
 *********************************************************/
void soundDist(sndEffects value, BYTE mx, BYTE my) {
  if (!threadsGetContext()) {
    clientSoundDist(value, mx, my);
  } else {
    serverCoreSoundDist(value, mx, my);
  }
}

/*********************************************************
 *NAME:          screenGetBuildings
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the buildings Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
building *screenGetBuildings() {
  if (!threadsGetContext()) {
    return clientGetBuildings();
  }

  return serverCoreGetBuildings();
}

/*********************************************************
 *NAME:          screenGetExplosions
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the explosions Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
explosions *screenGetExplosions() {
  if (!threadsGetContext()) {
    return clientGetExplosions();
  }

  return serverCoreGetExplosions();
}

/*********************************************************
 *NAME:          screenGetFloodFill
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the floodfill object
 *
 *ARGUMENTS:
 *
 *********************************************************/
floodFill *screenGetFloodFill() {
  if (!threadsGetContext()) {
    return clientGetFloodFill();
  }

  return serverCoreGetFloodFill();
}

/*********************************************************
 *NAME:          screenGetGrass
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the grass object
 *
 *ARGUMENTS:
 *
 *********************************************************/
grass *screenGetGrass() {
  if (!threadsGetContext()) {
    return clientGetGrass();
  }

  return serverCoreGetGrass();
}

/*********************************************************
 *NAME:          screenGetMines
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the mines object
 *
 *ARGUMENTS:
 *
 *********************************************************/
mines *screenGetMines() {
  if (!threadsGetContext()) {
    return clientGetMines();
  }

  return serverCoreGetMines();
}

/*********************************************************
 *NAME:          screenGetMinesExp
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the mines explosion object
 *
 *ARGUMENTS:
 *
 *********************************************************/
minesExp *screenGetMinesExp() {
  if (!threadsGetContext()) {
    return clientGetMinesExp();
  }
  return serverGetMinesExp();
}

/*********************************************************
 *NAME:          screenGetRubble
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the rubble object
 *
 *ARGUMENTS:
 *
 *********************************************************/
RubbleState *screenGetRubble() {
  if (!threadsGetContext()) {
    return clientGetRubble();
  }
  return serverCoreGetRubble();
}

/*********************************************************
 *NAME:          screenGetSwamp
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the swamp object
 *
 *ARGUMENTS:
 *
 *********************************************************/
swamp *screenGetSwamp() {
  if (!threadsGetContext()) {
    return clientGetSwamp();
  }
  return serverCoreGetSwamp();
}

/*********************************************************
 *NAME:          screenGetTankExplosions
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the tank explosions object
 *
 *ARGUMENTS:
 *
 *********************************************************/
tkExplosion *screenGetTankExplosions() {
  if (!threadsGetContext()) {
    return clientGetTankExplosions();
  }
  return serverCoreGetTankExplosions();
}

/*********************************************************
 *NAME:          screenGetNetPnb
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the Pnb Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
netPnbContext *screenGetNetPnb() {
  if (!threadsGetContext()) {
    return clientGetNetPnb();
  }
  return serverCoreGetNetPnb();
}

/*********************************************************
 *NAME:          screenGetNetMnt
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the Pnb Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
netMntContext *screenGetNetMnt() {
  if (!threadsGetContext()) {
    return clientGetNetMnt();
  }
  return serverCoreGetNetMnt();
}

/*********************************************************
 *NAME:          screenSetInStartFind
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/11/99
 *LAST MODIFIED: 28/11/99
 *PURPOSE:
 *  Sets if we are entering of leaving the find a start
 *  position
 *
 *ARGUMENTS:
 *  entering - true if entering, false if leaving
 *********************************************************/
void screenSetInStartFind(bool entering) {
  if (!threadsGetContext()) {
    clientSetInStartFind(entering);
  }
}

/*********************************************************
 *NAME:          screenGetInStartFind
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/11/99
 *LAST MODIFIED: 28/11/99
 *PURPOSE:
 *  Gets if we are entering or leaving the find a start
 *  position
 *
 *ARGUMENTS:
 *
 *********************************************************/
bool screenGetInStartFind() {
  if (!threadsGetContext()) {
    return clientGetInStartFind();
  }
  return false;
}

/*********************************************************
 *NAME:          screenGetGameType
 *AUTHOR:        John Morrison
 *CREATION DATE: 17/12/03
 *LAST MODIFIED: 17/12/03
 *PURPOSE:
 *  Returns a pointer to the game type
 *
 *ARGUMENTS:
 *
 *********************************************************/
gameType *screenGetGameType() {
  if (!threadsGetContext()) {
    return clientGetGameType();
  }
  return serverCoreGetGameType();
}

bool backendGetContext() { return threadsGetContext(); }

/*
typedef enum {
  BsTrees,
  BsRoad,
  BsBuilding,
  BsPillbox,
  BsMine
} buildSelect;
*/

/*********************************************************
 *NAME:          getBuildCurrentSelect
 *AUTHOR:        Chris Lesnieski
 *CREATION DATE: 10/05/2009
 *LAST MODIFIED: 10/05/2009
 *PURPOSE:
 *  Will get the currently selected build action.
 *
 *ARGUMENTS:
 *
 *********************************************************/
buildSelect getBuildCurrentSelect() { return BsCurrent; }

/*********************************************************
 *NAME:          setBuildCurrentSelect
 *AUTHOR:        Chris Lesnieski
 *CREATION DATE: 10/05/2009
 *LAST MODIFIED: 10/05/2009
 *PURPOSE:
 *  When passed in a type of build parameter, will set the
 *  BsCurrent variable, which holds the current build action.
 *
 *ARGUMENTS:
 *  bs - this should denote one of the build buttons
 *       on the left hand side of the screen.
 *
 *********************************************************/
void setBuildCurrentSelect(buildSelect bs) {
  /* Didn't send the right type of variable */
  if ((bs != BsTrees) && (bs != BsRoad) && (bs != BsBuilding) &&
      (bs != BsPillbox) && (bs != BsMine)) {
    return;
  } else {
    BsCurrent = bs;
  }
}

/*********************************************************
 *NAME:          Timer Functions
 *AUTHOR:        Minhiriath
 *CREATION DATE: 1/1/2009
 *LAST MODIFIED: 1/1/2009
 *PURPOSE:
 *  Deals with all timer functions in winbolo client and server.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void initWinboloTimer(void) {
#ifdef _WIN32
  timeBeginPeriod(1);
#else
  // inset linux stuff here
  gettimeofday(&start_time, nullptr);
#endif
}

DWORD winboloTimer(void) {
#ifdef _WIN32
  return timeGetTime();
#else
  // insert linux stuff here
  // return SDL_GetTicks();
  struct timeval t;
  gettimeofday(&t, nullptr);
  return (DWORD)((t.tv_sec - start_time.tv_sec) * 1000000 +
                 (t.tv_usec - start_time.tv_usec)) /
         1000;
#endif
}

void endWinboloTimer(void) {
#ifdef _WIN32
  timeEndPeriod(1);
#else
  // insert linux stuff here
#endif
}
