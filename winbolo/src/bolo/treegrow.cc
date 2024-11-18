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
*Name:          Tree Growth
*Filename:      treegrow.c
*Author:        John Morrison
*Creation Date: 22/1/99
*Last Modified: 22/1/99
*Purpose:
*  Responsible for storing and calculating tree growth
*********************************************************/


#include <stdlib.h>
#include <time.h>
#include "global.h"
#include "types.h"
#include "bolo_map.h"
#include "pillbox.h"
#include "bases.h"
#include "grass.h"
#include "swamp.h"
#include "rubble.h"
#include "screen.h"
#include "backend.h"
#include "treegrow.h"

/* The tree growth structure */
static BYTE treeGrowX;
static BYTE treeGrowY;
static int treeGrowTime;
static int treeGrowScore;
static WORD seed;

/*********************************************************
*NAME:          treeGrowCreate
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
*  Sets up the treeGrow data structure
*
*ARGUMENTS:
*
*********************************************************/
void treeGrowCreate(void) {
//  srand((unsigned int) time(NULL));
  seed = (WORD) rand();
  treeGrowTime = TREEGROW_INITIAL_TIME;
  treeGrowScore = TREEGROW_INITIAL_SCORE;
}

/*********************************************************
*NAME:          treeGrowDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
*  Destroys and frees memory for the treeGrow data 
*  structure
*
*ARGUMENTS:
*
*********************************************************/
void treeGrowDestroy(void) {
  treeGrowTime = TREEGROW_INITIAL_TIME;
}

/*********************************************************
*NAME:          treeGrowAddItem
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
*  Adds or updates an item in the treeGrow data structure.
*  Replaces the score if the score is greater then the
*  one that is already there and resets the timer to zero
*
*ARGUMENTS:
*  x     - X co-ord
*  y     - Y co-ord
*  score - The score of the item
*********************************************************/
void treeGrowAddItem(BYTE x, BYTE y, int score) {
  if (score > treeGrowScore && score > TREE_NO_GROW) {
    treeGrowScore = score;
    treeGrowX = x;
    treeGrowY = y;
    treeGrowTime = TREEGROW_TIME;
  }
}

/*********************************************************
*NAME:          treeGrowUpdate
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
*  Game tick has happened. Calculate the growth scores 
*  Build new trees etc.
*
*ARGUMENTS:
*  mp - Pointer to the map structure
*  pb - Pointer to the pillboxes structure
*  bs - Pointer to the bases structure
*********************************************************/
void treeGrowUpdate(map *mp, pillboxes *pb, bases *bs) {
  BYTE x;        /* The X and Y co-ordinates of the item to add */
  BYTE y;
	int i;
	WORD old, rnd;

  rnd = seed;
  /* Get a random map square to calculate for */
  for (i=0; i<16; i++) { old=rnd; rnd<<=1; if (old & 0x8000) rnd ^= 0x1083L; }
  seed = rnd;
  x = (BYTE) (rnd >> 8);
  y = (BYTE) rnd ;


  if (pillsExistPos(pb, x, y) == false && basesExistPos(bs, x,y) == false && mapIsMine(mp, x, y) == false && mapGetPos(mp, x, y) != DEEP_SEA) {
    treeGrowAddItem(x, y,treeGrowCalcScore(mp, pb, bs, x, y));
  }
    
  /* Update the times and grow trees etc */
  treeGrowCheckGrowTree(mp, pb, bs);
}

/*********************************************************
*NAME:          treeGrowCalcScore
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
* Calculates the tree growth score for the a specific 
* map location.
*
*ARGUMENTS:
*  mp - Pointer to the map structure
*  pb - Pointer to the pillboxes structure
*  bs - Pointer to the bases structure
*  mx - Map X Position
*  my - Map Y Position
*********************************************************/
int treeGrowCalcScore(map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my) {
  int returnValue;  /* value to return */
  BYTE pos;         /* The current Position */

  returnValue = 0;
  pos = mapGetPos(mp, mx, my);
  if (pos >= MINE_START && pos <= MINE_END) {
    pos -= MINE_SUBTRACT;
  }
  if (pos != FOREST && pos != RIVER && pos != BUILDING && pos != HALFBUILDING && pillsExistPos(pb, mx, my) == false && basesExistPos(bs, mx, my) == false) {
    /* OK to do calculation */
    returnValue += treeGrowCalcSquare(mp, pb, bs, mx, my);
    returnValue += treeGrowCalcSquare(mp, pb, bs, (BYTE) (mx-1), (BYTE) (my-1));
    returnValue += treeGrowCalcSquare(mp, pb, bs, (BYTE) (mx-1), my);
    returnValue += treeGrowCalcSquare(mp, pb, bs, (BYTE) (mx-1), (BYTE) (my+1));
    returnValue += treeGrowCalcSquare(mp, pb, bs, mx, (BYTE) (my-1));
    returnValue += treeGrowCalcSquare(mp, pb, bs, mx, (BYTE) (my+1));
    returnValue += treeGrowCalcSquare(mp, pb, bs, (BYTE) (mx+1), (BYTE) (my-1));
    returnValue += treeGrowCalcSquare(mp, pb, bs, (BYTE) (mx+1), my);
    returnValue += treeGrowCalcSquare(mp, pb, bs, (BYTE) (mx+1), (BYTE) (my-1));
    if (returnValue < 0) {
      returnValue = 0;
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          treeGrowCheckGrowTree
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
* Called once every tick to see if a tree should be grown
* and removed from the data structure.
*
*ARGUMENTS:
*  mp - Pointer to the map structure
*  pb - Pointer to the pillboxes structure
*  bs - Pointer to the bases structure
*********************************************************/
void treeGrowCheckGrowTree(map *mp, pillboxes *pb, bases *bs) {
  BYTE pos;           /* The map terrain at the current position */
  BYTE newPos;        /* The new position to set the terrain to */

  treeGrowTime--;
    /* Check for fill and remove from data structure */
  if (treeGrowTime <= 0) {
    treeGrowScore = TREEGROW_INITIAL_SCORE;
    treeGrowTime = TREEGROW_INITIAL_TIME;
    pos = mapGetPos(mp, treeGrowX, treeGrowY);
    if (pos != RIVER && pos != BUILDING && pos != HALFBUILDING && pillsExistPos(pb, treeGrowX, treeGrowY) == false && basesExistPos(bs, treeGrowX, treeGrowY) == false) {
      newPos = FOREST;
      if (pos >= MINE_START && pos <= MINE_END) {
        newPos += MINE_FOREST;
      }
      mapSetPos(mp, treeGrowX, treeGrowY, newPos, true, false);
      /* Remove Items from grass/swamp/rubble data stuctures */
      switch (pos) {
      case GRASS:
        grassRemovePos(screenGetGrass(), treeGrowX, treeGrowY);
        break;
      case SWAMP:
        swampRemovePos(screenGetSwamp(), treeGrowX, treeGrowY);
        break;
      case RUBBLE:
        rubbleRemovePos(screenGetRubble(), treeGrowX, treeGrowY);
        break;
      default:
        /* Do nothing */
        break;
      }
    }
  }
}

/*********************************************************
*NAME:          treeGrowCalcSquare
*AUTHOR:        John Morrison
*CREATION DATE: 22/1/99
*LAST MODIFIED: 22/1/99
*PURPOSE:
* Calculates the tree growth score for a specific square
*
*ARGUMENTS:
*  mp - Pointer to the map structure
*  pb - Pointer to the pillboxes structure
*  bs - Pointer to the bases structure
*  mx - Map X position
*  my - Map Y position
*********************************************************/
int treeGrowCalcSquare(map *mp, pillboxes *pb, bases *bs, BYTE mx, BYTE my) {
  int returnValue; /* Value to return */
  BYTE terrain;    /* The terrain at the position */


  returnValue = 0; //TREE_GROW_ROAD;
  if (pillsExistPos(pb, mx, my) == true) {
    returnValue = TREE_GROW_ROAD;
  } else if (basesExistPos(bs, mx, my) == true) {
      returnValue = TREE_GROW_ROAD;
  } else {
    terrain = mapGetPos(mp, mx, my);
    if (terrain >= MINE_START && terrain <= MINE_END) {
      returnValue = TREE_GROW_MINE;
      terrain -= MINE_START;
    }
     
    switch (terrain) {
      case FOREST:
        returnValue += TREE_GROW_FOREST;
        break;
      case GRASS:
        returnValue +=  TREE_GROW_GRASS;
        break;
      case RIVER:
        returnValue += TREE_GROW_RIVER; 
        break;
      case BOAT:  
        returnValue += TREE_GROW_BOAT;
        break;
      case DEEP_SEA:
        returnValue += TREE_GROW_DEEP_SEA;
        break;
      case SWAMP:  
        returnValue += TREE_GROW_DEEP_SWAMP;
        break;
      case RUBBLE:
        returnValue += TREE_GROW_DEEP_RUBBLE;  
        break;
      case BUILDING:  
        returnValue += TREE_GROW_BUILDING;  
        break;
      case HALFBUILDING:  
        returnValue += TREE_GROW_HALF_BUILDING;  
        break;
      case CRATER:
        returnValue += TREE_GROW_CRATER;
        break;
      case ROAD:
        returnValue +=  TREE_GROW_ROAD;
        break;
      default:
        returnValue +=  TREE_GROW_ROAD;
        break;
    }
  }
  return returnValue;
}
