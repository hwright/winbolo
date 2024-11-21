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

/*********************************************************
 *NAME:          buildingCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 30/12/98
 *PURPOSE:
 *  Sets up the building data structure
 *
 *ARGUMENTS:
 * bld - Pointer to the buildings object
 *********************************************************/
void buildingCreate(building *bld) { *bld = new buildingObj; }

/*********************************************************
 *NAME:          buildingDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 30/12/98
 *PURPOSE:
 *  Destroys and frees memory for the building data structure
 *
 *ARGUMENTS:
 * bld - Pointer to the buildings object
 *********************************************************/
void buildingDestroy(building *bld) { delete *bld; }

/*********************************************************
 *NAME:          buildingeAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 25/04/01
 *PURPOSE:
 *  Adds an item to the building data structure.
 *  If it already exists returns the terrain type of the
 *  item and decrements its lifetime.
 *
 *ARGUMENTS:
 *  bld   - Pointer to the buildings object
 *  x     - X co-ord
 *  y     - Y co-ord
 *********************************************************/
BYTE buildingAddItem(building *bld, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};
  if (auto it = (*bld)->buildings_.find(pos); it != (*bld)->buildings_.end()) {
    it->second -= 1;
    if (it->second == BUILDING_DEATH) {
      (*bld)->buildings_.erase(it);
      return RUBBLE;
    }
  } else {
    (*bld)->buildings_[pos] = BUILDING_LIFE;
  }

  return HALFBUILDING;
}

/*********************************************************
 *NAME:          buildingRemovePos
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/99
 *LAST MODIFIED: 18/1/99
 *PURPOSE:
 *  Removes an item from the building data structure if it
 *  exists at a specific loaction. Otherwise the function
 *  does nothing
 *
 *ARGUMENTS:
 *  bld   - Pointer to the buildings object
 *  x     - X co-ord
 *  y     - Y co-ord
 *********************************************************/
void buildingRemovePos(building *bld, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};
  (*bld)->buildings_.erase(pos);
}
