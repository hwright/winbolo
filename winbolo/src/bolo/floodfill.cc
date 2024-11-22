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

void FloodState::addItem(MapPoint pos) {
  if (!floods_.contains(pos)) {
    floods_[pos] = FLOOD_FILL_WAIT;
  }
}

void FloodState::Update(map *mp, pillboxes *pb, bases *bs) {
  std::vector<MapPoint> removed;

  for (auto &[pos, time] : floods_) {
    if (time > 0) {
      time -= 1;
    } else {
      checkFill(mp, pb, bs, pos);
      // `erase` invalidates our iterator, so we have to clean things up
      // separately.
      removed.push_back(pos);
    }
  }

  // Remove our deleted elements
  for (auto pos : removed) {
    floods_.erase(pos);
  }
}

void FloodState::checkFill(map *mp, pillboxes *pb, bases *bs, MapPoint pos) {
  // Squares around
  BYTE above;
  BYTE below;
  BYTE leftPos;
  BYTE rightPos;

  above = mapGetPos(mp, pos.N());
  below = mapGetPos(mp, pos.S());
  leftPos = mapGetPos(mp, pos.W());
  rightPos = mapGetPos(mp, pos.E());

  // Check for pills, bases etc. If found change to non crater / water */
  if (pillsExistPos(pb, pos.N())) {
    above = ROAD;
  } else if (basesExistPos(bs, pos.N())) {
    above = ROAD;
  }

  if (pillsExistPos(pb, pos.S())) {
    below = ROAD;
  } else if (basesExistPos(bs, pos.S())) {
    below = ROAD;
  }

  if (pillsExistPos(pb, pos.W())) {
    leftPos = ROAD;
  } else if (basesExistPos(bs, pos.W())) {
    leftPos = ROAD;
  }

  if (pillsExistPos(pb, pos.E())) {
    rightPos = ROAD;
  } else if (basesExistPos(bs, pos.E())) {
    rightPos = ROAD;
  }

  if (leftPos == DEEP_SEA || leftPos == BOAT || leftPos == RIVER ||
      rightPos == DEEP_SEA || rightPos == BOAT || rightPos == RIVER ||
      above == DEEP_SEA || above == RIVER || above == BOAT ||
      below == DEEP_SEA || below == BOAT || below == RIVER) {
    /* Do fill */
    mapSetPos(mp, pos, RIVER, false, false);
    minesRemoveItem(screenGetMines(), pos.x, pos.y);

    /* Add items if craters */
    if (leftPos == CRATER || leftPos == MINE_CRATER) {
      addItem(pos.W());
    }
    if (rightPos == CRATER || rightPos == MINE_CRATER) {
      addItem(pos.E());
    }
    if (above == CRATER || above == MINE_CRATER) {
      addItem(pos.N());
    }
    if (below == CRATER || below == MINE_CRATER) {
      addItem(pos.S());
    }
    screenReCalc();
  }
}
