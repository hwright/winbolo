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
 *Name:          Screen Tanks
 *Filename:      screenTanks.h
 *Author:        John Morrison
 *Creation Date: 15/2/99
 *Last Modified: 26/1/02
 *Purpose:
 *  Responsable for tanks on the screen
 *********************************************************/

#ifndef SCREENTANKS_H
#define SCREENTANKS_H

#include <vector>

#include "global.h"
#include "tank.h"

namespace bolo {

struct ScreenTank {
  // The map coordinate it is on
  MapPoint pos;
  uint8_t px;         // The pixel offset from the left it is on
  uint8_t py;         // The pixel offset from the top it is on
  uint8_t frame;      // The direction it is facing
  uint8_t playerNum;  // The player Number
  std::string playerName;
};

struct ScreenTankList {
  uint8_t numTanksScreen;  // The number of tanks on screen
  std::vector<ScreenTank> tanks;

  //  Prepare the screenTanks data structure prior to  displaying
  //
  // ARGUMENTS:
  //  tnk      - Pointer to your tank data structure
  //  leftPos  - Left bounds of the screen
  //  rightPos - Right bounds of the screen
  //  top      - Top bounds of the screen
  //  bottom   - Bottom bounds of the screen
  void prepare(tank *tnk, uint8_t leftPos, uint8_t rightPos, uint8_t top,
               uint8_t bottom);
};

// The tank type relative to ourselves, good, neutral or evil
enum class tankAlliance { tankNone, tankSelf, tankAllie, tankEvil };

}  // namespace bolo

#endif /* SCREENTANKS_H */
