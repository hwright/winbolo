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

// Abstract interface for functions called by the back end

#ifndef _FRONTEND_H
#define _FRONTEND_H

#include "global.h"
#include "screen.h"
#include "screenbullet.h"
#include "screengunsight.h"
#include "screenlgm.h"
#include "screenmines.h"
#include "screentank.h"
#include "sound_effects.h"
#include "types.h"
#ifdef _WIN32
#include "../gui/resource.h"
#else
#include "../gui/linresource.h"
#endif
#include "../gui/lang.h"

namespace bolo {

// The interface which a frontend must implement
class Frontend {
 public:
  virtual ~Frontend() = default;

  // Called when the tanks status bars need to be updated
  //
  // ARGUMENTS:
  //  shells  - Number of shells
  //  mines   - Number of mines
  //  armour  - Amount of armour
  //  trees   - Amount of trees
  virtual void updateTankStatusBars(uint8_t shells, uint8_t mines,
                                    uint8_t armour, uint8_t trees) = 0;

  // Called when the tanks status bars need to be updated
  //
  // ARGUMENTS:
  //  shells  - Number of shells
  //  mines   - Number of mines
  //  armour  - Amount of armour
  virtual void updateBaseStatusBars(uint8_t shells, uint8_t mines,
                                    uint8_t armour) = 0;

  // Play a sound effect if sounds are enabled.
  //
  // ARGUMENTS:
  //  value - The sound effect to play
  virtual void playSound(sndEffects value) = 0;

  // Update the Window View
  //
  // ARGUMENTS:
  //  value      - Pointer to the sceen structure
  //  mineView   - Pointer to the screen mine structure
  //  tks        - Pointer to the screen tank structure
  //  gs         - pointer to the screen gunsight structure
  //  lgms       - Pointer to the screen builder structure
  //  srtDelay   - Start delay. If this is greater then 0
  //               Then the delay screen should be drawn
  //  isPillView - TRUE if we are in pillbox view
  //  tank       - Pointer to the player's tank structure
  //  edgeX      - X Offset for smooth scrolling
  //  edgeY      - Y Offset for smooth scrolling
  virtual void drawMainScreen(screen *value, screenMines *mineView,
                              screenTanks *tks, screenGunsight *gs,
                              screenBullets *sBullet, screenLgm *lgms,
                              long srtDelay, bool isPillView, tank *tank,
                              int edgeX, int edgeY) = 0;

  // Set the pillbox status for a particular pillbox
  //
  // ARGUMENTS:
  //  pillNum - The base number to draw (1-16)
  //  pa      - The allience of the pillbox
  virtual void statusPillbox(uint8_t pillNum, pillAlliance pb) = 0;

  // Set the tank status for a particular tank
  //
  // ARGUMENTS:
  //  tankNum - The tank number to draw (1-16)
  //  ts      - The allience of the tank
  virtual void statusTank(uint8_t tankNum, tankAlliance ts) = 0;

  // Set the base status for a particular base
  //
  // ARGUMENTS:
  //  pillNum - The base number to draw (1-16)
  //  bs      - The allience of the pillbox
  virtual void statusBase(uint8_t baseNum, baseAlliance bs) = 0;

  // The messages must be drawn on the screen
  //
  // ARGUMENTS:
  //  top    - The top line to write
  //  bottom - The bottom line to write
  virtual void messages(std::string_view top, std::string_view bottom) = 0;

  // The tank kills/deaths must be updated
  //
  // ARGUMENTS:
  //  kills  - The number of kills the tank has.
  //  deaths - The number of times the tank has died
  virtual void killsDeaths(int kills, int deaths) = 0;

  // The man status must be updated
  //
  // ARGUMENTS:
  //  isDead - Is the man dead
  //  angle  - The angle the man is facing
  virtual void manStatus(bool isDead, TURNTYPE angle) = 0;

  // Clears the man status (ie man is in tank)
  virtual void manClear(void) = 0;
};

}  // namespace bolo

/*********************************************************
 *NAME:          frontEndGameOver
 *AUTHOR:        John Morrison
 *CREATION DATE: 29/1/99
 *LAST MODIFIED: 29/1/99
 *PURPOSE:
 *  Time limit is up. The game is over
 *
 *ARGUMENTS:
 *
 *********************************************************/
void frontEndGameOver(void);

/*********************************************************
 *NAME:          frontEndClearPlayer
 *AUTHOR:        John Morrison
 *CREATION DATE: 4/2/99
 *LAST MODIFIED: 4/2/99
 *PURPOSE:
 * Clears a player name from the menu and disables it.
 *
 *ARGUMENTS:
 *  value - The player number to clear
 *********************************************************/
void frontEndClearPlayer(playerNumbers value);

/*********************************************************
 *NAME:          frontEndSetPlayer
 *AUTHOR:        John Morrison
 *CREATION DATE: 4/2/99
 *LAST MODIFIED: 4/2/99
 *PURPOSE:
 * Sets a player name in the menu and enables it.
 *
 *ARGUMENTS:
 *  value - The player number to set
 *  str   - String identifier of the name
 *********************************************************/
void frontEndSetPlayer(playerNumbers value, char *str);

/*********************************************************
 *NAME:          frontEndDrawDownload
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/3/99
 *LAST MODIFIED: 13/12/99
 *PURPOSE:
 * A screen redraw request has been made but we are still
 * downloading network data. Draw progress line instead.
 *
 *ARGUMENTS:
 *  justBlack - TRUE if we want just a black screen
 *********************************************************/
void frontEndDrawDownload(bool justBlack);

/*********************************************************
 *NAME:          frontEndSetPlayerCheckState
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/3/99
 *LAST MODIFIED: 27/3/99
 *PURPOSE:
 * Checks/unchecks a player
 *
 *ARGUMENTS:
 * value     - The player number
 * isChecked - Is the item checked
 *********************************************************/
void frontEndSetPlayerCheckState(playerNumbers value, bool isChecked);

/*********************************************************
 *NAME:          frontEndEnableRequestAllyMenu
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/11/99
 *LAST MODIFIED: 1/11/99
 *PURPOSE:
 * Request to enable/disable the request alliance menu
 * item
 *
 *ARGUMENTS:
 *  enabled - TRUE for enabled/FALSE for diabled
 *********************************************************/
void frontEndEnableRequestAllyMenu(bool enabled);

/*********************************************************
 *NAME:          frontEndEnableRequestMenu
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/11/99
 *LAST MODIFIED: 1/11/99
 *PURPOSE:
 * Request to enable/disable the leave alliance menu item
 *
 *ARGUMENTS:
 *  enabled - TRUE for enabled/FALSE for diabled
 *********************************************************/
void frontEndEnableLeaveAllyMenu(bool enabled);

/*********************************************************
 *NAME:          frontEndShowGunsight
 *AUTHOR:        John Morrison
 *CREATION DATE: 4/1/00
 *LAST MODIFIED: 4/1/00
 *PURPOSE:
 * Set the front end whether the gunsight is visible or not
 * (Called by auto show/hide gunsight being triggered)
 *
 *ARGUMENTS:
 *  isShown - Is the gunsight shown or not
 *********************************************************/
void frontEndShowGunsight(bool isShown);

/*********************************************************
 *NAME:          frontEndRedrawAll
 *AUTHOR:        Minhiriath
 *CREATION DATE: 16/3/2009
 *LAST MODIFIED: 16/3/2009
 *PURPOSE:
 *  called windowRedrawAll()
 *
 *ARGUMENTS:
 *  none
 *********************************************************/
void frontEndRedrawAll(void);

/*********************************************************
 *NAME:          frontEndTutorial
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/5/00
 *LAST MODIFIED: 27/5/00
 *PURPOSE:
 * Set the front end that we have reached a new position
 *
 *ARGUMENTS:
 *  pos - Y Position on the map
 *********************************************************/
bool frontEndTutorial(BYTE pos);

#endif /* _FRONT_END_H */
