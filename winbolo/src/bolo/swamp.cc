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
 *Filename:      swamp.c
 *Author:        John Morrison
 *Creation Date: 5/1/99
 *Last Modified: 25/04/01
 *Purpose:
 *  Responsable for tracking lifetime of swamp when shot
 *  from a boat
 *********************************************************/

#include "swamp.h"

#include "global.h"

/*********************************************************
 *NAME:          swampCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 5/1/99
 *LAST MODIFIED: 5/1/99
 *PURPOSE:
 *  Sets up the swamp data structure
 *
 *ARGUMENTS:
 *
 *********************************************************/
void swampCreate(swamp *swmp) { *swmp = new swampObj; }

/*********************************************************
 *NAME:          swampDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 5/1/99
 *LAST MODIFIED: 5/1/99
 *PURPOSE:
 *  Destroys and frees memory for the swamp data structure
 *
 *ARGUMENTS:
 *
 *********************************************************/
void swampDestroy(swamp *swmp) { delete *swmp; }

/*********************************************************
 *NAME:          swampAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 5/1/99
 *LAST MODIFIED: 25/04/01
 *PURPOSE:
 *  Adds an item to the swamp data structure.
 *  If it already exists returns the terrain type of the
 *  item and decrements its lifetime.
 *
 *ARGUMENTS:
 *  x     - X co-ord
 *  y     - Y co-ord
 *********************************************************/
BYTE swampAddItem(swamp *swmp, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};

  if (auto it = (*swmp)->swamps_.find(pos); it != (*swmp)->swamps_.end()) {
    it->second -= 1;
    if (it->second == SWAMP_DEATH) {
      (*swmp)->swamps_.erase(it);
      return SWAMP_DEATH_RETURN;
    }
  } else {
    (*swmp)->swamps_[pos] = SWAMP_LIFE;
  }

  return SWAMP;
}

/*********************************************************
 *NAME:          swampRemovePos
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/99
 *LAST MODIFIED: 18/1/99
 *PURPOSE:
 *  Removes an item from the swamp data structure if it
 *  exists at a specific loaction. Otherwise the function
 *  does nothing
 *
 *ARGUMENTS:
 *  x     - X co-ord
 *  y     - Y co-ord
 *********************************************************/
void swampRemovePos(swamp *swmp, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};
  (*swmp)->swamps_.erase(pos);
}
