/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024-     Hyrum Wright.
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

namespace bolo {

namespace {

// The time a tree exist before growing unless we find a better spot
const int TREEGROW_TIME = 3000;  // 3700

// Initial time delay before we build a tree. This will be overwritten
// before it gets called
const int INITIAL_TIME = 30000;
// Initial scores to beat
const int INITIAL_SCORE = -10000;

// Weights of the items
const int FOREST_WEIGHT = 100;         // 67  100
const int GRASS_WEIGHT = 25;           // 10 17  25
const int RIVER_WEIGHT = 2;            // 7  10
const int BOAT_WEIGHT = 1;             // 3  5
const int DEEP_SEA_WEIGHT = 0;         // 1
const int DEEP_SWAMP_WEIGHT = 2;       // 1
const int DEEP_RUBBLE_WEIGHT = -2;     // -2  -1
const int BUILDING_WEIGHT = -20;       // -7  -5
const int HALF_BUILDING_WEIGHT = -15;  // -7  -5
const int CRATER_WEIGHT = -2;          // -2  -1
const int ROAD_WEIGHT = -100;          // -18 * -14  -10
const int MINE_WEIGHT = -7;            // -7 -5

// Score must be greater then 0 for it to grow
const int TREE_NO_GROW = 0;

// Calculates the tree growth score for a specific square
//
// ARGUMENTS:
//  mp - Pointer to the map structure
//  pb - Pointer to the pillboxes structure
//  bs - Pointer to the bases structure
//  pos - Map position
int calcSquare(map *mp, pillboxes *pb, bases *bs, MapPoint pos) {
  int returnValue = 0;  // ROAD_WEIGHT;

  if (pillsExistPos(pb, pos.x, pos.y)) {
    returnValue = ROAD_WEIGHT;
  } else if (basesExistPos(bs, pos.x, pos.y)) {
    returnValue = ROAD_WEIGHT;
  } else {
    BYTE terrain = mapGetPos(mp, pos.x, pos.y);
    if (terrain >= MINE_START && terrain <= MINE_END) {
      returnValue = MINE_WEIGHT;
      terrain -= MINE_START;
    }

    switch (terrain) {
      case FOREST:
        returnValue += FOREST_WEIGHT;
        break;
      case GRASS:
        returnValue += GRASS_WEIGHT;
        break;
      case RIVER:
        returnValue += RIVER_WEIGHT;
        break;
      case BOAT:
        returnValue += BOAT_WEIGHT;
        break;
      case DEEP_SEA:
        returnValue += DEEP_SEA_WEIGHT;
        break;
      case SWAMP:
        returnValue += DEEP_SWAMP_WEIGHT;
        break;
      case RUBBLE:
        returnValue += DEEP_RUBBLE_WEIGHT;
        break;
      case BUILDING:
        returnValue += BUILDING_WEIGHT;
        break;
      case HALFBUILDING:
        returnValue += HALF_BUILDING_WEIGHT;
        break;
      case CRATER:
        returnValue += CRATER_WEIGHT;
        break;
      case ROAD:
        returnValue += ROAD_WEIGHT;
        break;
      default:
        returnValue += ROAD_WEIGHT;
        break;
    }
  }
  return returnValue;
}

}  // namespace

TreeGrowState::TreeGrowState() : time_(INITIAL_TIME), score_(INITIAL_SCORE) {}

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
    score_ = INITIAL_SCORE;
    time_ = INITIAL_TIME;
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
          screenGetSwamp()->removePos(seedling_);
          break;
        case RUBBLE:
          screenGetRubble()->removePos(seedling_);
          break;
        default:
          /* Do nothing */
          break;
      }
    }
  }
}

}  // namespace bolo
