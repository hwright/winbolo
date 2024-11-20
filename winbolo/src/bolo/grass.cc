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
 *Filename:      grass.c
 *Author:        John Morrison
 *Creation Date: 5/1/99
 *Last Modified: 25/04/01
 *Purpose:
 *  Responsable for tracking lifetime of grass when shot
 *  from a boat
 *********************************************************/

#include "grass.h"

#include "global.h"

/*********************************************************
 *NAME:          grassCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 5/1/99
 *LAST MODIFIED: 5/1/99
 *PURPOSE:
 *  Sets up the grass data structure
 *
 *ARGUMENTS:
 * grs - Pointer to the grass object
 *********************************************************/
void grassCreate(grass *grs) { *grs = new grassObj; }

/*********************************************************
 *NAME:          grassDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 5/1/99
 *LAST MODIFIED: 5/1/99
 *PURPOSE:
 *  Destroys and frees memory for the grass data structure
 *
 *ARGUMENTS:
 * grs - Pointer to the grass object
 *********************************************************/
void grassDestroy(grass *grs) { delete *grs; }

/*********************************************************
 *NAME:          grassAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 5/1/99
 *LAST MODIFIED: 25/04/01
 *PURPOSE:
 *  Adds an item to the grass data structure.
 *  If it already exists returns the terrain type of the
 *  item and decrements its lifetime.
 *
 *ARGUMENTS:
 *  grs   - Pointer to the grass object
 *  x     - X co-ord
 *  y     - Y co-ord
 *********************************************************/
BYTE grassAddItem(grass *grs, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};

  if (auto it = (*grs)->grasses_.find(pos); it != (*grs)->grasses_.end()) {
    it->second -= 1;
    if (it->second == GRASS_DEATH) {
      (*grs)->grasses_.erase(it);
      return GRASS_DEATH_RETURN;
    }
  } else {
    (*grs)->grasses_[pos] = GRASS_LIFE;
  }

  return GRASS;
}

void grassRemovePos(grass *grs, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};
  (*grs)->grasses_.erase(pos);
}
