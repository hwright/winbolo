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
 *Filename:      minesexp.h
 *Author:        John Morrison
 *Creation Date: 20/1/99
 *Last Modified: 3/10/00
 *Purpose:
 *  Responsible for tracking mine explosions, chain
 *  reactions and damage to tank from them.
 *********************************************************/

#ifndef MINEEXP_H
#define MINEEXP_H

#include "global.h"
#include "lgm.h"
#include "types.h"

/* Time between mine explosion and add removal checks */
#define MINES_EXPLOSION_WAIT 10

/* Type structure */

typedef struct minesExpObj *minesExp;
struct minesExpObj {
  std::unordered_map<MapPoint, uint8_t> explosions_;
};

/* Prototypes */

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
void minesExpCreate(minesExp *me);

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
void minesExpAddItem(minesExp *me, map *mp, BYTE x, BYTE y);

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
void minesExpDestroy(minesExp *me);

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
                    BYTE numLgm);

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
                       lgm **lgms, BYTE numLgm, BYTE mx, BYTE my);

#endif /* MINEEXP_H */
