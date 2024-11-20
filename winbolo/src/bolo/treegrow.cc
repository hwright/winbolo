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

#include "treegrow.h"

#include <stdlib.h>
#include <time.h>

#include "backend.h"
#include "bases.h"
#include "bolo_map.h"
#include "global.h"
#include "grass.h"
#include "pillbox.h"
#include "rubble.h"
#include "screen.h"
#include "swamp.h"
#include "types.h"

namespace {

// Calculates the tree growth score for a specific square
//
// ARGUMENTS:
//  mp - Pointer to the map structure
//  pb - Pointer to the pillboxes structure
//  bs - Pointer to the bases structure
//  pos - Map position
int calcSquare(map *mp, pillboxes *pb, bases *bs, MapPoint pos) {
  int returnValue; /* Value to return */
  BYTE terrain;    /* The terrain at the position */

  returnValue = 0;  // TREE_GROW_ROAD;
  if (pillsExistPos(pb, pos.x, pos.y)) {
    returnValue = TREE_GROW_ROAD;
  } else if (basesExistPos(bs, pos.x, pos.y)) {
    returnValue = TREE_GROW_ROAD;
  } else {
    terrain = mapGetPos(mp, pos.x, pos.y);
    if (terrain >= MINE_START && terrain <= MINE_END) {
      returnValue = TREE_GROW_MINE;
      terrain -= MINE_START;
    }

    switch (terrain) {
      case FOREST:
        returnValue += TREE_GROW_FOREST;
        break;
      case GRASS:
        returnValue += TREE_GROW_GRASS;
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
        returnValue += TREE_GROW_ROAD;
        break;
      default:
        returnValue += TREE_GROW_ROAD;
        break;
    }
  }
  return returnValue;
}

}  // namespace

TreeGrowState::TreeGrowState()
    : time_(TREEGROW_INITIAL_TIME), score_(TREEGROW_INITIAL_SCORE) {}

void TreeGrowState::addItem(MapPoint point, int score) {
  if (score > score_ && score > TREE_NO_GROW) {
    score_ = score;
    seedling_ = point;
    time_ = TREEGROW_TIME;
  }
}

void TreeGrowState::Update(map *mp, pillboxes *pb, bases *bs) {
  // Get a random map square to calculate for
  MapPoint candidate{
      .x = distribution_(generator_),
      .y = distribution_(generator_),
  };

  if (!pillsExistPos(pb, candidate.x, candidate.y) &&
      !basesExistPos(bs, candidate.x, candidate.y) &&
      !mapIsMine(mp, candidate.x, candidate.y) &&
      mapGetPos(mp, candidate.x, candidate.y) != DEEP_SEA) {
    addItem(candidate, calcScore(mp, pb, bs, candidate));
  }

  /* Update the times and grow trees etc */
  checkGrowTree(mp, pb, bs);
}

int TreeGrowState::calcScore(map *mp, pillboxes *pb, bases *bs,
                             MapPoint candidate) {
  int returnValue; /* value to return */
  BYTE pos;        /* The current Position */

  returnValue = 0;
  pos = mapGetPos(mp, candidate.x, candidate.y);
  if (pos >= MINE_START && pos <= MINE_END) {
    pos -= MINE_SUBTRACT;
  }
  if (pos != FOREST && pos != RIVER && pos != BUILDING && pos != HALFBUILDING &&
      !pillsExistPos(pb, candidate.x, candidate.y) &&
      !basesExistPos(bs, candidate.x, candidate.y)) {
    /* OK to do calculation */
    returnValue += calcSquare(mp, pb, bs, candidate);
    returnValue += calcSquare(mp, pb, bs, candidate.NW());
    returnValue += calcSquare(mp, pb, bs, candidate.N());
    returnValue += calcSquare(mp, pb, bs, candidate.NE());
    returnValue += calcSquare(mp, pb, bs, candidate.E());
    returnValue += calcSquare(mp, pb, bs, candidate.SE());
    returnValue += calcSquare(mp, pb, bs, candidate.S());
    returnValue += calcSquare(mp, pb, bs, candidate.SW());
    returnValue += calcSquare(mp, pb, bs, candidate.W());
    if (returnValue < 0) {
      returnValue = 0;
    }
  }
  return returnValue;
}

void TreeGrowState::checkGrowTree(map *mp, pillboxes *pb, bases *bs) {
  BYTE pos;    /* The map terrain at the current position */
  BYTE newPos; /* The new position to set the terrain to */

  time_--;
  /* Check for fill and remove from data structure */
  if (time_ <= 0) {
    score_ = TREEGROW_INITIAL_SCORE;
    time_ = TREEGROW_INITIAL_TIME;
    pos = mapGetPos(mp, seedling_.x, seedling_.y);
    if (pos != RIVER && pos != BUILDING && pos != HALFBUILDING &&
        !pillsExistPos(pb, seedling_.x, seedling_.y) &&
        !basesExistPos(bs, seedling_.x, seedling_.y)) {
      newPos = FOREST;
      if (pos >= MINE_START && pos <= MINE_END) {
        newPos += MINE_FOREST;
      }
      mapSetPos(mp, seedling_.x, seedling_.y, newPos, true, false);
      /* Remove Items from grass/swamp/rubble data stuctures */
      switch (pos) {
        case GRASS:
          grassRemovePos(screenGetGrass(), seedling_.x, seedling_.y);
          break;
        case SWAMP:
          swampRemovePos(screenGetSwamp(), seedling_.x, seedling_.y);
          break;
        case RUBBLE:
          rubbleRemovePos(screenGetRubble(), seedling_.x, seedling_.y);
          break;
        default:
          /* Do nothing */
          break;
      }
    }
  }
}
