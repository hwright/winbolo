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
 *Name:          Flood Fill
 *Filename:      floodfill.c
 *Author:        John Morrison
 *Creation Date: 19/1/99
 *Last Modified: 19/1/99
 *Purpose:
 *  Responsible for tracking flood fills caused by craters
 *  next to water.
 *********************************************************/

#include "floodfill.h"

#include "backend.h"
#include "bases.h"
#include "bolo_map.h"
#include "global.h"
#include "mines.h"
#include "pillbox.h"
#include "screen.h"

/*********************************************************
 *NAME:          floodCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/1/99
 *LAST MODIFIED: 19/1/99
 *PURPOSE:
 *  Sets up the flood fill data structure
 *
 *ARGUMENTS:
 *  ff - Pointer to the floodFill item
 *********************************************************/
void floodCreate(floodFill *ff) { *ff = new floodFillObj; }

/*********************************************************
 *NAME:          floodDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/1/99
 *LAST MODIFIED: 19/1/99
 *PURPOSE:
 *  Destroys and frees memory for the flood fill data
 *  structure
 *
 *ARGUMENTS:
 *  ff - Pointer to the floodFill item
 *********************************************************/
void floodDestroy(floodFill *ff) {
  delete *ff;
  *ff = nullptr;
}

/*********************************************************
 *NAME:          floodAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/1/99
 *LAST MODIFIED: 19/1/99
 *PURPOSE:
 *  Adds an item to the flood data structure.
 *
 *ARGUMENTS:
 *  ff - Pointer to the floodFill item
 *  x  - X co-ord
 *  y  - Y co-ord
 *********************************************************/
void floodAddItem(floodFill *ff, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};

  if (!(*ff)->floods_.contains(pos)) {
    (*ff)->floods_[pos] = FLOOD_FILL_WAIT;
  }
}

/*********************************************************
 *NAME:          floodUpdate
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/1/99
 *LAST MODIFIED: 21/1/99
 *PURPOSE:
 *  Game tick has happened. Update flooding
 *
 *ARGUMENTS:
 *  ff - Pointer to the floodFill item
 *  mp - Pointer to the map structure
 *  pb - Pointer to the pillboxes structure
 *  bs - Pointer to the bases structure
 *********************************************************/
void floodUpdate(floodFill *ff, map *mp, pillboxes *pb, bases *bs) {
  if (*ff == nullptr) return;

  std::vector<MapPoint> removed;

  for (auto &[pos, time] : (*ff)->floods_) {
    if (time > 0) {
      time -= 1;
    } else {
      floodCheckFill(ff, mp, pb, bs, pos.x, pos.y);
      // `erase` invalidates our iterator, so we have to clean things up
      // separately.
      removed.push_back(pos);
    }
  }

  // Remove our deleted elements
  for (auto pos : removed) {
    (*ff)->floods_.erase(pos);
  }
}

/*********************************************************
 *NAME:          floodCheckFill
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/1/99
 *LAST MODIFIED: 21/3/99
 *PURPOSE:
 *  Time to fill if required. Also if it does adds
 *  surrounding items to flood Data Structure.
 *
 *ARGUMENTS:
 *  ff - Pointer to the floodFill item
 *  mp - Pointer to the map structure
 *  pb - Pointer to the pillboxes structure
 *  bs - Pointer to the bases structure
 *  mx - Map X Position
 *  my - Map Y Position
 *********************************************************/
void floodCheckFill(floodFill *ff, map *mp, pillboxes *pb, bases *bs, BYTE mx,
                    BYTE my) {
  BYTE above; /* Squares around */
  BYTE below;
  BYTE leftPos;
  BYTE rightPos;

  above = mapGetPos(mp, mx, (BYTE)(my - 1));
  below = mapGetPos(mp, mx, (BYTE)(my + 1));
  leftPos = mapGetPos(mp, (BYTE)(mx - 1), my);
  rightPos = mapGetPos(mp, (BYTE)(mx + 1), my);

  /* Check for pills, bases etc. If found change to non crater / water */
  if (pillsExistPos(pb, mx, (BYTE)(my - 1))) {
    above = ROAD;
  } else if (basesExistPos(bs, mx, (BYTE)(my - 1))) {
    above = ROAD;
  }

  if (pillsExistPos(pb, mx, (BYTE)(my + 1))) {
    below = ROAD;
  } else if (basesExistPos(bs, mx, (BYTE)(my + 1))) {
    below = ROAD;
  }

  if (pillsExistPos(pb, (BYTE)(mx - 1), my)) {
    leftPos = ROAD;
  } else if (basesExistPos(bs, (BYTE)(mx - 1), my)) {
    leftPos = ROAD;
  }

  if (pillsExistPos(pb, (BYTE)(mx + 1), my)) {
    rightPos = ROAD;
  } else if (basesExistPos(bs, (BYTE)(mx - 1), my)) {
    rightPos = ROAD;
  }

  if (leftPos == DEEP_SEA || leftPos == BOAT || leftPos == RIVER ||
      rightPos == DEEP_SEA || rightPos == BOAT || rightPos == RIVER ||
      above == DEEP_SEA || above == RIVER || above == BOAT ||
      below == DEEP_SEA || below == BOAT || below == RIVER) {
    /* Do fill */
    mapSetPos(mp, mx, my, RIVER, false, false);
    minesRemoveItem(screenGetMines(), mx, my);

    /* Add items if craters */
    if (leftPos == CRATER || leftPos == MINE_CRATER) {
      floodAddItem(ff, (BYTE)(mx - 1), my);
    }
    if (rightPos == CRATER || rightPos == MINE_CRATER) {
      floodAddItem(ff, (BYTE)(mx + 1), my);
    }
    if (above == CRATER || above == MINE_CRATER) {
      floodAddItem(ff, mx, (BYTE)(my - 1));
    }
    if (below == CRATER || below == MINE_CRATER) {
      floodAddItem(ff, mx, (BYTE)(my + 1));
    }
    screenReCalc();
  }
}
