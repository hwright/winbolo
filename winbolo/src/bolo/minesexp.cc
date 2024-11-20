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
void minesExpCreate(minesExp *me) { *me = nullptr; }

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
  minesExp q;
  minesExp inc;
  bool found; /* Is the item found */

  inc = *me;
  found = false;

  while (!found && NonEmpty(inc)) {
    if (inc->x == x && inc->y == y) {
      found = true;
    }
    inc = MinesExpTail(inc);
  }

  /* If not found add a new item */
  if (!found) {
    q = new minesExpObj;
    q->x = x;
    q->y = y;
    q->time = MINES_EXPLOSION_WAIT;
    q->next = *me;
    q->prev = nullptr;
    if (NonEmpty(*me)) {
      (*me)->prev = q;
    }
    *me = q;
  }

  //  testPos = mapGetPos(mp, x, y);
  //  if (netGetType() != netSingle && threadsGetContext() == false && testPos
  //  >= MINE_START && testPos <= MINE_END) {
  //    mapSetPos(mp, x, y, CRATER, false);
  //    explosionsAddItem(x, y, 0, 0, EXPLOSION_START);
  //    soundDist(mineExplosionNear, x, y);
  //  }
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
void minesExpDestroy(minesExp *me) {
  minesExp q;

  while (!IsEmpty(*me)) {
    q = *me;
    *me = MinesExpTail(q);
    delete q;
  }
}

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
  minesExp position; /* Position throught the items */
  bool needUpdate;   /* Whether an update is needed or not */

  position = *me;

  while (NonEmpty(position)) {
    needUpdate = true;
    if (position->time > 0) {
      position->time--;
    } else {
      /* Check for fill and remove from data structure */
      needUpdate = false;
      minesExpCheckFill(me, mp, pb, bs, lgms, numLgm, position->x, position->y);
      minesRemoveItem(screenGetMines(), position->x, position->y);
      minesExpDeleteItem(me, &position);
    }

    /* Get the next Item */
    if (*me != nullptr && needUpdate) {
      position = MinesExpTail(position);
    } else {
      position = nullptr;
    }
  }
}

/*********************************************************
 *NAME:          minesExpDeleteItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/1/99
 *LAST MODIFIED: 12/1/99
 *PURPOSE:
 *  Deletes the value from the master list
 *
 *ARGUMENTS:
 *  me      - Pointer to the mines object
 *  value   - Pointer to the shells to delete. Also puts
 *            next shell its position
 *********************************************************/
void minesExpDeleteItem(minesExp *me, minesExp *value) {
  minesExp del; /* The item to delete */

  del = *value;
  (*value) = MinesExpTail(del);
  if (del->prev != nullptr) {
    del->prev->next = del->next;
  } else {
    /* Must be the first item - Move the master position along one */
    *me = ShellsTail(*me);
    if (NonEmpty(*me)) {
      (*me)->prev = nullptr;
    }
  }

  if (del->next != nullptr) {
    del->next->prev = del->prev;
  }
  delete del;
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
    floodAddItem(screenGetFloodFill(), mx, my);
    /* Remove Items from grass/swamp/rubble data stuctures */
    switch (pos - MINE_SUBTRACT) {
      case GRASS:
        grassRemovePos(screenGetGrass(), mx, my);
        break;
      case SWAMP:
        swampRemovePos(screenGetSwamp(), mx, my);
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
