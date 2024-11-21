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
 *Name:          Building
 *Filename:      building.h
 *Author:        John Morrison
 *Creation Date: 30/12/98
 *Last Modified: 04/10/03
 *Purpose:
 *  Responsable for tracking lifetime of Buildings.
 *  buildings can be shot 5 times before being destroyed
 *********************************************************/

#ifndef BUILDING_H
#define BUILDING_H

#include <unordered_map>

#include "global.h"

/* How manu shots it takes to destroy a building */
#define BUILDING_LIFE 4

/* Shells die when there length equals */
#define BUILDING_DEATH 0

class BuildingState {
 public:
  BuildingState() = default;

  // Move-only
  BuildingState(BuildingState &) = delete;
  BuildingState &operator=(BuildingState &) = delete;

  // Add a building item. If it already exists return
  // the terrain type of the item and decrements its lifetime.
  //
  // ARGUMENTS:
  //  pos - The building position
  BYTE addItem(MapPoint pos);

  // Remove a building item if it exists at a specific
  // location. Otherwise the function does nothing.
  //
  // ARGUMENTS:
  //  pos - The building position
  void removePos(MapPoint pos) { buildings_.erase(pos); }

 private:
  std::unordered_map<MapPoint, uint8_t> buildings_;
};

#endif /* BUILDING_H */
