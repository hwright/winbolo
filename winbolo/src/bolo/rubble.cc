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
 *Name:          Rubble
 *Filename:      rubble.c
 *Author:        John Morrison
 *Creation Date: 30/12/98
 *Last Modified: 25/04/01
 *Purpose:
 *  Responsable for tracking lifetime of rubble.
 *  buildings can be shot 5 times before being destroyed
 *********************************************************/

#include "rubble.h"

#include "global.h"

/*********************************************************
 *NAME:          rubbleCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 30/12/98
 *PURPOSE:
 *  Sets up the rubble data structure
 *
 *ARGUMENTS:
 *  rbl - Pointer to the rubbble object
 *********************************************************/
void rubbleCreate(rubble *rbl) { *rbl = new rubbleObj; }

/*********************************************************
 *NAME:          rubbleDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 30/12/98
 *PURPOSE:
 *  Destroys and frees memory for the rubble data structure
 *
 *ARGUMENTS:
 *  rbl - Pointer to the rubbble object
 *********************************************************/
void rubbleDestroy(rubble *rbl) { delete *rbl; }

/*********************************************************
 *NAME:          rubbleAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 25/04/01
 *PURPOSE:
 *  Adds an item to the rubble data structure.
 *  If it already exists returns the terrain type of the
 *  item and decrements its lifetime.
 *
 *ARGUMENTS:
 *  rbl - Pointer to the rubbble object
 *  x   - X co-ord
 *  y   - Y co-ord
 *********************************************************/
BYTE rubbleAddItem(rubble *rbl, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};
  if (auto it = (*rbl)->rubbles_.find(pos); it != (*rbl)->rubbles_.end()) {
    it->second -= 1;
    if (it->second == RUBBLE_DEATH) {
      rubbleRemovePos(rbl, pos.x, pos.y);
      return RIVER;
    }
  } else {
    (*rbl)->rubbles_[pos] = RUBBLE_LIFE;
  }

  return RUBBLE;
}

/*********************************************************
 *NAME:          rubbleRemovePos
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/99
 *LAST MODIFIED: 18/1/99
 *PURPOSE:
 *  Removes an item from the rubble data structure if it
 *  exists at a specific loaction. Otherwise the function
 *  does nothing
 *
 *ARGUMENTS:
 *  rbl - Pointer to the rubbble object
 *  x   - X co-ord
 *  y   - Y co-ord
 *********************************************************/
void rubbleRemovePos(rubble *rbl, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};
  (*rbl)->rubbles_.erase(pos);
}
