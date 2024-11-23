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
 *Name:          Mines Explosions
 *Filename:      minesexp.c
 *Author:        John Morrison
 *Creation Date: 20/01/99
 *Last Modified: 04/04/02
 *Purpose:
 *  Responsible for tracking mine explosions, chain
 *  reactions and damage to tank from them.
 *********************************************************/

#include "minesexp.h"

#include <vector>

#include "backend.h"
#include "explosions.h"
#include "floodfill.h"
#include "global.h"
#include "grass.h"
#include "lgm.h"
#include "mines.h"
#include "netmt.h"
#include "network.h"
#include "players.h"
#include "rubble.h"
#include "screen.h"
#include "sounddist.h"
#include "swamp.h"
#include "types.h"

/*********************************************************
 *NAME:          minesExpCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/1/99
 *LAST MODIFIED: 20/1/99
 *PURPOSE:
 *  Sets up the MinesExp data structure
 *
 *ARGUMENTS:
 * me - Pointer to the mines object
 *********************************************************/
void minesExpCreate(minesExp *me) { *me = new minesExpObj; }

/*********************************************************
 *NAME:          minesExpAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/1/99
 *LAST MODIFIED: 2/11/99
 *PURPOSE:
 *  Adds an item to the minesExp data structure.
 *
 *ARGUMENTS:
 *  me - Pointer to the mines object
 *  mp - Map Structure
 *  x  - X co-ord
 *  y  - Y co-ord
 *********************************************************/
void minesExpAddItem(minesExp *me, map *mp, BYTE x, BYTE y) {
  MapPoint pos{.x = x, .y = y};
  if (!(*me)->explosions_.contains(pos)) {
    (*me)->explosions_.insert({pos, MINES_EXPLOSION_WAIT});
  }
}

/*********************************************************
 *NAME:          minesExpDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/1/99
 *LAST MODIFIED: 20/1/99
 *PURPOSE:
 *  Destroys and frees memory for the minesExp data
 *  structure
 *
 *ARGUMENTS:
 *  me - Pointer to the mines object
 *********************************************************/
void minesExpDestroy(minesExp *me) { delete *me; }

/*********************************************************
 *NAME:          minesExpUpdate
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/1/99
 *LAST MODIFIED: 3/10/00
 *PURPOSE:
 *  Game tick has happened. Update flooding
 *
 *ARGUMENTS:
 *  me     - Pointer to the mines object
 *  mp     - Pointer to the map structure
 *  pb     - Pointer to the pillboxes structure
 *  bs     - Pointer to the bases structure
 *  lgms   - Array of lgms
 *  numLgm - Number of lgms in the array
 *********************************************************/
void minesExpUpdate(minesExp *me, map *mp, pillboxes *pb, bases *bs, lgm **lgms,
                    BYTE numLgm) {
  std::vector<MapPoint> removals;

  for (auto &[pos, time] : (*me)->explosions_) {
    if (time > 0) {
      time -= 1;
    } else {
      minesExpCheckFill(me, mp, pb, bs, lgms, numLgm, pos.x, pos.y);
      minesRemoveItem(screenGetMines(), pos.x, pos.y);
      removals.push_back(pos);
    }
  }

  for (auto pos : removals) {
    (*me)->explosions_.erase(pos);
  }
}

/*********************************************************
 *NAME:          minesExpCheckFill
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/01/99
 *LAST MODIFIED: 04/04/02
 *PURPOSE:
 *  Time to fill if required. Also if it does adds
 *  surrounding items to minesExp Data Structure.
 *
 *ARGUMENTS:
 *  me     - Pointer to the mines object
 *  mp     - Pointer to the map structure
 *  pb     - Pointer to the pillboxes structure
 *  bs     - Pointer to the bases structure
 *  lgms   - Array of lgms
 *  numLgm - Number of lgms in the array
 *  mx     - Map X Position
 *  my     - Map Y Position
 *********************************************************/
void minesExpCheckFill(minesExp *me, map *mp, pillboxes *pb, bases *bs,
                       lgm **lgms, BYTE numLgm, BYTE mx, BYTE my) {
  BYTE pos;      /* Map pos being worked on */
  BYTE count;    /* Looping variable */
  bool abovePos; /* Are squares around mines? */
  bool belowPos;
  bool leftPos;
  bool rightPos;

  abovePos = false;
  belowPos = false;
  leftPos = false;
  rightPos = false;
  pos = mapGetPos(mp, mx, my);
  minesRemoveItem(screenGetMines(), mx, my);
  if (pos >= MINE_START && pos <= MINE_END) {
    mapSetPos(mp, mx, my, CRATER, false, false);
    soundDist(mineExplosionNear, mx, my);
    count = 1;
    while (count <= numLgm) {
      lgmDeathCheck(lgms[count - 1], mp, pb, bs,
                    (WORLD)((mx << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE),
                    (WORLD)((my << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE),
                    NEUTRAL);
      count++;
    }
    screenCheckTankMineDamage(mx, my);
    explosionsAddItem(screenGetExplosions(), mx, my, 0, 0, EXPLOSION_START);
    screenGetFloodFill()->addItem(MapPoint{.x = mx, .y = my});
    /* Remove Items from grass/swamp/rubble data stuctures */
    switch (pos - MINE_SUBTRACT) {
      case GRASS:
        screenGetGrass()->removePos(MapPoint{.x = mx, .y = my});
        break;
      case SWAMP:
        screenGetSwamp()->removePos(MapPoint{.x = mx, .y = my});
        break;
      case RUBBLE:
        screenGetRubble()->removePos(MapPoint{.x = mx, .y = my});
        break;
    }
    pos = mapGetPos(mp, mx, (BYTE)(my - 1));
    if (pos >= MINE_START && pos <= MINE_END) {
      abovePos = true;
    }
    pos = mapGetPos(mp, mx, (BYTE)(my + 1));
    if (pos >= MINE_START && pos <= MINE_END) {
      belowPos = true;
    }
    pos = mapGetPos(mp, (BYTE)(mx - 1), my);
    if (pos >= MINE_START && pos <= MINE_END) {
      leftPos = true;
    }
    pos = mapGetPos(mp, (BYTE)(mx + 1), my);
    if (pos >= MINE_START && pos <= MINE_END) {
      rightPos = true;
    }

    /* Add items if craters */
    if (leftPos) {
      minesExpAddItem(me, mp, (BYTE)(mx - 1), my);
    }
    if (rightPos) {
      minesExpAddItem(me, mp, (BYTE)(mx + 1), my);
    }
    if (abovePos) {
      minesExpAddItem(me, mp, mx, (BYTE)(my - 1));
    }
    if (belowPos) {
      minesExpAddItem(me, mp, mx, (BYTE)(my + 1));
    }
    screenReCalc();
  }
}
