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
 *  Responsable for tracking lifetime of buildings.
 *  buildings can be shot 5 times before being destroyed
 *********************************************************/

#include "building.h"

#include "global.h"

BYTE BuildingState::addItem(MapPoint pos) {
  if (auto it = buildings_.find(pos); it != buildings_.end()) {
    it->second -= 1;
    if (it->second == BUILDING_DEATH) {
      buildings_.erase(it);
      return RUBBLE;
    }
  } else {
    buildings_[pos] = BUILDING_LIFE;
  }

  return HALFBUILDING;
}
