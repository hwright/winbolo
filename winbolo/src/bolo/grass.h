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
 *Name:          Grass
 *Filename:      grass.h
 *Author:        John Morrison
 *Creation Date: 5/1/99
 *Last Modified: 18/1/99
 *Purpose:
 *  Responsable for tracking lifetime of grass when shot
 *  from a boat
 *********************************************************/

#ifndef GRASS_H
#define GRASS_H

#include <unordered_map>

#include "global.h"

/* How manu shots it takes to destroy a peice of grass */
#define GRASS_LIFE 4

/* Shells die when there length equals */
#define GRASS_DEATH 0

/* What grass truns into when it dies */
#define GRASS_DEATH_RETURN SWAMP

class GrassState {
 public:
  GrassState() = default;

  // Move-only
  GrassState(GrassState &) = delete;
  GrassState &operator=(GrassState &) = delete;

  // Add an item to the grass data structure.
  // If it already exists returns the terrain type of the
  // item and decrements its lifetime.
  //
  // ARGUMENTS:
  //  pos - The position of the object.
  BYTE addItem(MapPoint pos);

  // Remove an item from the grass data structure if it
  // exists at a specific loaction. Otherwise the function
  // does nothing.
  //
  // ARGUMENTS:
  //  pos - The position of the object.
  void removePos(MapPoint pos) { grasses_.erase(pos); }

 private:
  std::unordered_map<MapPoint, uint8_t> grasses_;
};

#endif /* GRASS_H */
