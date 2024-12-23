/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024-     Hyrum Wright.
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

#include <format>
#include <optional>

#include "global.h"
#include "screen.h"
#include "screentypes.h"
#include "sound_effects.h"
#include "types.h"

namespace bolo {

struct ManStatus {
  bool is_dead;
  TURNTYPE angle;
};

// Conslidated tank status struct
struct TankSupply {
  uint8_t shells = 0;
  uint8_t mines = 0;
  uint8_t armor = 0;
  uint8_t trees = 0;
};

// Consolidated base supply struct
struct BaseSupply {
  uint8_t shells = 0;
  uint8_t mines = 0;
  uint8_t armor = 0;
};

// The interface which a frontend must implement
class Frontend {
 public:
  virtual ~Frontend() = default;

  // Called when the tanks status bars need to be updated
  //
  // ARGUMENTS:
  //  status  - The tank status struct
  virtual void updateTankSupplyBars(TankSupply tank_status) = 0;

  // Called when the tanks status bars need to be updated
  //
  // ARGUMENTS:
  //  status  - The base status struct
  virtual void updateBaseSupplyBars(BaseSupply base_status) = 0;

  // Play a sound effect if sounds are enabled.
  //
  // ARGUMENTS:
  //  value - The sound effect to play
  virtual void playSound(sndEffects value) = 0;

  // Update the Window View
  //
  // ARGUMENTS:
  //  value      - Pointer to the sceen structure
  //  tks        - Pointer to the screen tank structure
  //  gs         - pointer to the screen gunsight structure
  //  lgms       - Pointer to the screen builder structure
  //  srtDelay   - Start delay. If this is greater then 0
  //               Then the delay screen should be drawn
  //  isPillView - TRUE if we are in pillbox view
  //  edgeX      - X Offset for smooth scrolling
  //  edgeY      - Y Offset for smooth scrolling
  virtual void drawMainScreen(ScreenTiles tiles, ScreenTankList tks,
                              std::optional<ScreenGunsight> gunsight,
                              ScreenBulletList sBullets, ScreenLgmList lgms,
                              long srtDelay, bool isPillView, int edgeX,
                              int edgeY) = 0;

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
  virtual void setManStatus(std::optional<ManStatus> status) = 0;

  // Time limit is up. The game is over
  virtual void gameOver(void) = 0;

  // Clears a player name from the menu and disables it.
  //
  // ARGUMENTS:
  //  value - The player number to clear
  virtual void clearPlayer(playerNumbers value) = 0;

  // Sets a player name in the menu and enables it.
  //
  // ARGUMENTS:
  //  value - The player number to set
  //  str   - String identifier of the name
  virtual void setPlayer(playerNumbers value, std::string_view str) = 0;

  // A screen redraw request has been made but we are still
  // downloading network data. Draw progress line instead.
  //
  // ARGUMENTS:
  //  justBlack - TRUE if we want just a black screen
  virtual void drawDownload(bool justBlack) = 0;

  // Checks/unchecks a player
  //
  // ARGUMENTS:
  //  value     - The player number
  //  isChecked - Is the item checked
  virtual void setPlayerCheckState(playerNumbers value, bool isChecked) = 0;

  // Selected the appropriate indent
  //
  // ARGUMENTS:
  //  old_val  - The previous value
  //  new_val  - The current value
  virtual void selectIndent(buildSelect old_val, buildSelect new_val) = 0;

  // Request to enable/disable the request alliance menu item
  //
  // ARGUMENTS:
  //  enabled - TRUE for enabled/FALSE for diabled
  virtual void enableRequestAllyMenu(bool enabled) = 0;

  // Request to enable/disable the leave alliance menu item
  //
  // ARGUMENTS:
  //  enabled - TRUE for enabled/FALSE for diabled
  virtual void enableLeaveAllyMenu(bool enabled) = 0;

  // Set the front end whether the gunsight is visible or not
  // (Called by auto show/hide gunsight being triggered)
  //
  // ARGUMENTS:
  //  isShown - Is the gunsight shown or not
  virtual void showGunsight(bool isShown) = 0;

  // Force a complete window redraw.
  virtual void drawAll(void) = 0;

  // Set the front end that we have reached a new position
  //
  // ARGUMENTS:
  //  pos - Y Position on the map
  virtual bool tutorial(uint8_t pos) = 0;

  // Output the given error text in a suitable venue.
  virtual void error(std::string_view text) = 0;
  template <class... Args>

  // Output the provided text (formatted per std::format) to a suitable
  // location.
  void error(std::format_string<Args...> fmt, Args&&... args) {
    print_error(std::format(fmt, args...));
  }
};

}  // namespace bolo

#endif /* _FRONT_END_H */
