/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024-     Hyrum Wright
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

#include <stdio.h>

#include "../bolo/frontend.h"
#include "../bolo/global.h"
#include "../bolo/log.h"
#include "../bolo/messages.h"
#include "../bolo/screen.h"
#include "servernet.h"

/*********************************************************
 *NAME:          screenReCalc
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 30/12/98
 *PURPOSE:
 *  Recalculates the screen data
 *
 *ARGUMENTS:
 *
 *********************************************************/
void screenReCalc(void) { return; }

/*********************************************************
 *NAME:          clientSoundDist
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
void clientSoundDist(bolo::sndEffects value, BYTE mx, BYTE my) { return; }

/*********************************************************
 *NAME:          serverCoreSoundDist
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
void serverCoreSoundDist(bolo::sndEffects value, BYTE mx, BYTE my) {
  BYTE logMessageType; /* Log item type */
  bool wantLog;        /* Do we want to log this? */

  wantLog = true;
  switch (value) {
    case bolo::sndEffects::shootSelf:
    case bolo::sndEffects::shootNear:
    case bolo::sndEffects::shootFar:
      logMessageType = log_SoundShoot;
      break;

    case bolo::sndEffects::shotTreeNear:
    case bolo::sndEffects::shotTreeFar:
      logMessageType = log_SoundHitTree;
      break;

    case bolo::sndEffects::shotBuildingNear:
    case bolo::sndEffects::shotBuildingFar:
      logMessageType = log_SoundHitWall;
      break;

    case bolo::sndEffects::hitTankNear:
    case bolo::sndEffects::hitTankFar:
    case bolo::sndEffects::hitTankSelf:
      logMessageType = log_SoundHitTank;
      break;
    case bolo::sndEffects::bubbles:
    case bolo::sndEffects::tankSinkNear:
    case bolo::sndEffects::tankSinkFar:
      wantLog = false;
      break;
    case bolo::sndEffects::bigExplosionNear:
      logMessageType = log_SoundBigExplosion;
      break;
    case bolo::sndEffects::bigExplosionFar:
      logMessageType = log_SoundBigExplosion;
      break;
    case bolo::sndEffects::farmingTreeNear:
    case bolo::sndEffects::farmingTreeFar:
      logMessageType = log_SoundFarm;
      break;
    case bolo::sndEffects::manBuildingNear:
    case bolo::sndEffects::manBuildingFar:
      logMessageType = log_SoundBuild;
      break;
    case bolo::sndEffects::manDyingNear:
    case bolo::sndEffects::manDyingFar:
      logMessageType = log_SoundManDie;
      break;

    case bolo::sndEffects::manLayingMineNear:
      logMessageType = log_SoundMineLay;
      break;

    case bolo::sndEffects::mineExplosionNear:
    case bolo::sndEffects::mineExplosionFar:
      logMessageType = log_SoundMineExplode;
      break;
  }

  if (wantLog) {
    logAddEvent((logitem)logMessageType, mx, my, 0, 0, 0, nullptr);
  }
}

/*********************************************************
 *NAME:          netGetType
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/2/99
 *LAST MODIFIED: 21/2/99
 *PURPOSE:
 * Returns the network game type being played
 *
 *ARGUMENTS:
 *
 *********************************************************/
netType netGetType(void) {
  return (netType)0;  // FIXME
}

/*********************************************************
 *NAME:          screenTankView
 *AUTHOR:        John Morrison
 *CREATION DATE: 3/2/99
 *LAST MODIFIED: 3/2/99
 *PURPOSE:
 * Frontend has requested a tank view.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void screenTankView() { return; }

/*********************************************************
 *NAME:          clientCenterTank
 *AUTHOR:        John Morrison
 *CREATION DATE: 3/1/99
 *LAST MODIFIED: 3/1/99
 *PURPOSE:
 *  Centers the screen around the tank.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void clientCenterTank() { return; }

/*********************************************************
 *NAME:          screenGetTankWorld
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/99
 *LAST MODIFIED: 18/1/99
 *PURPOSE:
 * Gets the tank world co-ordinates
 *
 *ARGUMENTS:
 *  x - Pointer to hold World X Co-ordinates
 *  y - Pointer to hold World Y Co-ordinates
 *********************************************************/
void screenGetTankWorld(WORLD *x, WORLD *y) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: screenGetTankWorld", "");
  return;
}

/*********************************************************
 *NAME:          screenIsItemInTrees
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/2/99
 *LAST MODIFIED: 19/2/99
 *PURPOSE:
 * Returns whether a item is surrounded by trees.
 *
 *ARGUMENTS:
 *  bmx - X position
 *  bmy - Y position
 *********************************************************/
bool screenIsItemInTrees(WORLD bmx, WORLD bmy) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: screenIsItemInTrees", "");
  return true;
}

/*********************************************************
 *NAME:          screenGetPlayerName
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/2/99
 *LAST MODIFIED: 1/2/99
 *PURPOSE:
 * Copies the player name into the value passed
 *
 *ARGUMENTS:
 *  value - String to hold copy of the player name
 *********************************************************/
void screenGetPlayerName(char *value) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: screenGetPlayerName", "");
  return;
}

/*********************************************************
 *NAME:          screenCheckTankMineDamage
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/1/99
 *LAST MODIFIED: 21/1/99
 *PURPOSE:
 * Check for damage to a tank by a mine being set off.
 *
 *ARGUMENTS:
 *  mx - X Map Co-ordinate
 *  my - Y Map Co-ordinate
 *********************************************************/
void screenCheckTankMineDamage(BYTE mx, BYTE my) { return; }

/*********************************************************
 *NAME:          netGetStatus
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/2/99
 *LAST MODIFIED: 26/2/99
 *PURPOSE:
 * Returns the status of the network
 *
 *ARGUMENTS:
 *
 *********************************************************/
netStatus netGetStatus(void) { return netFailed; }

/*********************************************************
 *NAME:          netRemovePlayer
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/3/99
 *LAST MODIFIED: 21/3/99
 *PURPOSE:
 * Removes a player from the netplayers structure
 *
 *ARGUMENTS:
 *  playerNum - The player number to remove.
 *********************************************************/
void netRemovePlayer(BYTE playerNum) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global,
             "DANGEROUS: netRemovePlayer - Something MUST go here", "");
  return;
}

void netMessageSendPlayer(BYTE playerNum, BYTE destPlayer, char *message) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global,
             "DANGEROUS: netMessageSendPlayer - Something MUST go here", "");
  return;
}

/*********************************************************
 *NAME:          netRequestAlliance
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/11/99
 *LAST MODIFIED: 1/11/99
 *PURPOSE:
 * Sends a request alliance message.
 *
 *ARGUMENTS:
 *  playerNum - Player who is leaving alliance
 *  requestTo - Who we are requesting to
 *********************************************************/
void netRequestAlliance(BYTE playerNum, BYTE reqestTo) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: netRequestAlliance", "");
}

/*********************************************************
 *NAME:          screenPillAlliance
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 21/12/98
 *PURPOSE:
 *  Returns the pill alliance of a particular pill for
 *  status drawing.
 *
 *ARGUMENTS:
 *  pillNum - The pillbox number to get
 *********************************************************/
pillAlliance screenPillAlliance(BYTE pillNum) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: screenPillAlliance", "");
  return (pillAlliance)0;
}

/*********************************************************
 *NAME:          screenBaseAlliance
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 21/12/98
 *PURPOSE:
 *  Returns the base alliance of a particular base for
 *  drawing.
 *
 *ARGUMENTS:
 *  baseNum - The base number to get
 *********************************************************/
baseAlliance screenBaseAlliance(BYTE baseNum) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: screenBaseAlliance", "");
  return (baseAlliance)0;
}

/*********************************************************
 *NAME:          screenGetNumPlayers
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/1/99
 *LAST MODIFIED: 26/1/99
 *PURPOSE:
 * Returns the number of players in the game
 *
 *ARGUMENTS:
 *
 *********************************************************/
BYTE screenGetNumPlayers() {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: screenGetNumPlayers", "");
  return 0;
}

/*********************************************************
 *NAME:          screenChangeOwnership
 *AUTHOR:        John Morrison
 *CREATION DATE: 2/11/99
 *LAST MODIFIED: 2/11/99
 *PURPOSE:
 * Called because we have lost our connection to the server
 * Set all of our allies stuff to be owned by us.
 *
 *ARGUMENTS:
 *  oldOwner - The old Owner of the stuff
 *********************************************************/
void screenChangeOwnership(BYTE oldOwner) {
  // This code should never be called by the server.
  messageAdd(bolo::messageType::global, "DANGEROUS: screenChangeOwnership", "");
}

/*********************************************************
 *NAME:          screenMoveViewOffsetLeft
 *AUTHOR:        John Morrison
 *CREATION DATE: 2/11/99
 *LAST MODIFIED: 2/11/99
 *PURPOSE:
 * Moves our view left or right depending on the argument
 *
 *ARGUMENTS:
 *  isLeft - true for left, false for right
 *********************************************************/
void screenMoveViewOffsetLeft(bool isLeft) {}

/*********************************************************
 *NAME:          screenMoveViewOffsetUp
 *AUTHOR:        John Morrison
 *CREATION DATE: 2/11/99
 *LAST MODIFIED: 2/11/99
 *PURPOSE:
 * Moves our view up or down depending on the argument
 *
 *ARGUMENTS:
 *  isup - true for up, false for dpwm
 *********************************************************/
void screenMoveViewOffsetUp(bool isUp) {}

bool screenTankScroll() { return false; }

/*********************************************************
 *NAME:          screenAddBrainObject
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/11/99
 *LAST MODIFIED: 28/11/99
 *PURPOSE:
 *  Adds a brain object to the list of brain objects
 *
 *ARGUMENTS:
 *  object - The type of the object
 *  wx     - X position of the object
 *  wy     - Y position of the object
 *  idNum  - Objects identifier number
 *  dir    - Direction of the object
 *  info   - Object info
 *********************************************************/
void screenAddBrainObject(unsigned short object, WORLD wx, WORLD wy,
                          unsigned short idNum, BYTE dir, BYTE info) {}

/*********************************************************
 *NAME:          clientSetInStartFind
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
void clientSetInStartFind(bool entering) {}

bool clientGetInStartFind() { return false; }

void netErrorOccured() {}

/*********************************************************
 *NAME:          screenTankStopCarryingPill
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/6/00
 *LAST MODIFIED: 21/6/00
 *PURPOSE:
 * Someone else has picked up a pill. We should check that
 * we aren't carrying it ourselves and if so drop it (The
 * server said so) because this can lead to desync problems
 *
 *ARGUMENTS:
 *  message - The message text
 *********************************************************/
void screenTankStopCarryingPill(BYTE itemNum) { return; }

/*********************************************************
 *NAME:          netRequestStartPosition
 *AUTHOR:        John Morrison
 *CREATION DATE: 2/4/01
 *LAST MODIFIED: 2/4/01
 *PURPOSE:
 *  Request a new starting position off the server.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void netRequestStartPosition() { return; }

/* These functions never get called */

/*********************************************************
 *NAME:          clientNumBases
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 21/12/98
 *PURPOSE:
 *  Returns the number of bases
 *
 *ARGUMENTS:
 *
 *********************************************************/
BYTE clientNumBases(void) { return 0; }

/*********************************************************
 *NAME:          clientNumPills
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 21/12/98
 *PURPOSE:
 *  Returns the number of pillboxes
 *
 *ARGUMENTS:
 *
 *********************************************************/
BYTE clientNumPills(void) { return 0; }

/*********************************************************
 *NAME:          clientGetNumNeutralBases
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/2/99
 *LAST MODIFIED: 21/2/99
 *PURPOSE:
 * Returns the number of neutral bases
 *
 *ARGUMENTS:
 *
 *********************************************************/
BYTE clientGetNumNeutralBases(void) { return 0; }

/*********************************************************
 *NAME:          clientMapIsMine
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/11/99
 *LAST MODIFIED: 31/11/99
 *PURPOSE:
 * Returns whether a mines exists at a map square
 *
 *ARGUMENTS:
 *  bmx - Map X position
 *  bmy - Map X position
 *********************************************************/
bool clientMapIsMine(BYTE bmx, BYTE bmy) { return 0; }

/*********************************************************
 *NAME:          clientGetTankFromPlayer
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
tank *clientGetTankFromPlayer(BYTE playerNum) { return nullptr; }

/*********************************************************
 *NAME:          clientGetTankPlayer
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/10/99
 *LAST MODIFIED: 31/10/99
 *PURPOSE:
 * Returns this tanks player number
 *
 *ARGUMENTS:
 *  value - Pointer to the tank
 *********************************************************/
BYTE clientGetTankPlayer(tank *value) { return 0; }

/*********************************************************
 *NAME:          clientGetLgmFromPlayerNum
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
lgm *clientGetLgmFromPlayerNum(BYTE playerNum) { return nullptr; }

/*********************************************************
 *NAME:          clientGetTankWorldFromLgm
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
void clientGetTankWorldFromLgm(lgm *lgmans, WORLD *x, WORLD *y) { return; }

/*********************************************************
 *NAME:          clientCheckTankRange
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/04/01
 *LAST MODIFIED: 9/04/01
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
bool clientCheckTankRange(BYTE x, BYTE y, BYTE playerNum, double distance) {
  return false;
}

/*********************************************************
 *NAME:          clientCheckPillsRange
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
bool clientCheckPillsRange(BYTE xValue, BYTE yValue, BYTE playerNum,
                           double distance) {
  return false;
}

/*********************************************************
 *NAME:          clientGetRandStart
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
void clientGetRandStart(BYTE *mx, BYTE *my, TURNTYPE *dir) { return; }

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
void clientMessageAdd(bolo::messageType msgType, std::string_view top,
                      std::string_view bottom) {
  return;
}

/*********************************************************
 *NAME:          clientTankInView
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/8/99
 *LAST MODIFIED:  8/9/00
 *PURPOSE:
 * Returns true in a client.
 *
 *ARGUMENTS:
 * playerNum - PlayerNum to check
 * checkX    - X Position to check
 * checkY    - Y Position to check
 *********************************************************/
bool clientTankInView(BYTE playerNum, BYTE checkX, BYTE checkY) {
  return false;
}

/*********************************************************
 *NAME:          clientGetPlayers
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/02/03
 *LAST MODIFIED: 01/03/03
 *PURPOSE:
 * Returns the players object
 *
 *ARGUMENTS:
 *
 *********************************************************/
players *clientGetPlayers() { return nullptr; }

/*********************************************************
 *NAME:          clientBasesMigrate
 *AUTHOR:        Minhiriath
 *CREATION DATE: 15/03/2009
 *LAST MODIFIED: 15/03/2009
 *PURPOSE:
 * migrates the clients bases
 *
 *ARGUMENTS:
 *
 *********************************************************/
void clientBasesMigrate(BYTE playerNumOldOwner, BYTE playerNumNewOwner) {
  // do nothing here.
}

/*********************************************************
 *NAME:          clientPillsMigratePlanted
 *AUTHOR:        Minhiriath
 *CREATION DATE: 15/03/2009
 *LAST MODIFIED: 15/03/2009
 *PURPOSE:
 * migrates the clients planted pills
 *
 *ARGUMENTS:
 *
 *********************************************************/
void clientPillsMigratePlanted(BYTE playerNumOldOwner, BYTE playerNumNewOwner) {
  // do nothing here.
}

/*********************************************************
 *NAME:          clientGetBuildings
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the buildings Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::BuildingState *clientGetBuildings() { return nullptr; }

/*********************************************************
 *NAME:          clientGetExplosions
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the explosions Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::ExplosionTracker *clientGetExplosions() { return nullptr; }

/*********************************************************
 *NAME:          clientGetFloodFill
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the floodfill object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::FloodFill *clientGetFloodFill() { return nullptr; }

/*********************************************************
 *NAME:          clientGetGrass
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the grass object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::GrassState *clientGetGrass() { return nullptr; }

/*********************************************************
 *NAME:          clientGetMines
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the mines object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::MineTracker *clientGetMines() { return nullptr; }

/*********************************************************
 *NAME:          clientGetMinesExp
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the mines explosion object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::MineExplosionTracker *clientGetMinesExp() { return nullptr; }

/*********************************************************
 *NAME:          clientGetRubble
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the rubble object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::RubbleState *clientGetRubble() { return nullptr; }

/*********************************************************
 *NAME:          clientGetSwamp
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the swamp object
 *
 *ARGUMENTS:
 *
 *********************************************************/
bolo::SwampState *clientGetSwamp() { return nullptr; }

/*********************************************************
 *NAME:          clientGetTankExplosions
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the tank explosions object
 *
 *ARGUMENTS:
 *
 *********************************************************/
tkExplosion *clientGetTankExplosions() { return nullptr; }

/*********************************************************
 *NAME:          clientGetNetPnb
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the Pnb Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
netPnbContext *clientGetNetPnb() { return nullptr; }

/*********************************************************
 *NAME:          clientGetNetPnb
 *AUTHOR:        John Morrison
 *CREATION DATE: 04/10/03
 *LAST MODIFIED: 04/10/03
 *PURPOSE:
 *  Returns a pointer to the Pnb Object
 *
 *ARGUMENTS:
 *
 *********************************************************/
netMntContext *clientGetNetMnt() { return nullptr; }

/*********************************************************
 *NAME:          clientGetGameType
 *AUTHOR:        John Morrison
 *CREATION DATE: 17/12/03
 *LAST MODIFIED: 17/12/03
 *PURPOSE:
 *  Returns a pointer to the game type
 *
 *ARGUMENTS:
 *
 *********************************************************/
gameType *clientGetGameType() { return nullptr; }
