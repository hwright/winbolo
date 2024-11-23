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

#include "floodfill.h"

#include "backend.h"
#include "bases.h"
#include "bolo_map.h"
#include "global.h"
#include "mines.h"
#include "pillbox.h"
#include "screen.h"

namespace bolo {

namespace {

// Time between flood fill removal checks
const int FILL_WAIT = 16;

}  // namespace

void FloodFill::addItem(MapPoint pos) {
  if (!floods_.contains(pos)) {
    floods_.insert({pos, FILL_WAIT});
  }
}

void FloodFill::Update(map *mp, pillboxes *pb, bases *bs) {
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

void FloodFill::checkFill(map *mp, pillboxes *pb, bases *bs, MapPoint pos) {
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
    screenGetMines()->removeItem(pos);

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

}  // namespace bolo
