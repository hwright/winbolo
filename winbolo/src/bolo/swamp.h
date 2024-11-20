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
 *Name:          Swamp
 *Filename:      swamp.h
 *Author:        John Morrison
 *Creation Date: 5/1/99
 *Last Modified: 18/1/99
 *Purpose:
 *  Responsable for tracking lifetime of swamp when shot
 *  from a boat
 *********************************************************/

#ifndef SWAMP_H
#define SWAMP_H

#include <unordered_map>

#include "global.h"

/* How many shots it takes to destroy a peice of grass this is set to 3 becuase
 * for some reason that makes winbolo kill a swamp in 4 shots. 4 shots is what
 * its supposed to be */
#define SWAMP_LIFE 3

/* Shells die when there length equals */
#define SWAMP_DEATH 0

/* What grass truns into when it dies */
#define SWAMP_DEATH_RETURN RIVER

class SwampState {
 public:
  // Add an item of swamp.
  // If one already exists, return the terrain type of the
  //  item and decrements its lifetime.
  //
  // ARGUMENTS:
  //  pos - The swamp position
  BYTE addItem(MapPoint pos);

  // Remove an item of swamp if it exists at a specific
  // location.
  //
  // ARGUMENTS:
  //  pos - The swamp position
  void removePos(MapPoint pos);

 private:
  std::unordered_map<MapPoint, uint8_t> swamps_;
};

#endif /* SWAMP_H */
