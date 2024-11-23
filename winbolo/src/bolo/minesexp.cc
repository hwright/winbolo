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

namespace bolo {

namespace {

// Time between mine explosion and add removal checks
const int WAIT_TIME = 10;

}  // namespace

void MineExplosionTracker::addItem(MapPoint pos) {
  if (!explosions_.contains(pos)) {
    explosions_.insert({pos, WAIT_TIME});
  }
}

void MineExplosionTracker::Update(map *mp, pillboxes *pb, bases *bs, lgm **lgms,
                                  BYTE numLgm) {
  std::vector<MapPoint> removals;

  for (auto &[pos, time] : explosions_) {
    if (time > 0) {
      time -= 1;
    } else {
      checkFill(mp, pb, bs, lgms, numLgm, pos);
      minesRemoveItem(screenGetMines(), pos.x, pos.y);
      removals.push_back(pos);
    }
  }

  for (auto pos : removals) {
    explosions_.erase(pos);
  }
}

void MineExplosionTracker::checkFill(map *mp, pillboxes *pb, bases *bs,
                                     lgm **lgms, BYTE numLgm, MapPoint pos) {
  BYTE terrain;  /* Map pos being worked on */
  BYTE count;    /* Looping variable */

  terrain = mapGetPos(mp, pos);
  minesRemoveItem(screenGetMines(), pos.x, pos.y);
  if (terrain >= MINE_START && terrain <= MINE_END) {
    mapSetPos(mp, pos, CRATER, false, false);
    soundDist(mineExplosionNear, pos.x, pos.y);
    count = 1;
    while (count <= numLgm) {
      lgmDeathCheck(lgms[count - 1], mp, pb, bs,
                    (WORLD)((pos.x << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE),
                    (WORLD)((pos.y << M_W_SHIFT_SIZE) + MAP_SQUARE_MIDDLE),
                    NEUTRAL);
      count++;
    }
    screenCheckTankMineDamage(pos.x, pos.y);
    explosionsAddItem(screenGetExplosions(), pos.x, pos.y, 0, 0,
                      EXPLOSION_START);
    screenGetFloodFill()->addItem(pos);
    /* Remove Items from grass/swamp/rubble data stuctures */
    switch (terrain - MINE_SUBTRACT) {
      case GRASS:
        screenGetGrass()->removePos(pos);
        break;
      case SWAMP:
        screenGetSwamp()->removePos(pos);
        break;
      case RUBBLE:
        screenGetRubble()->removePos(pos);
        break;
    }
    terrain = mapGetPos(mp, pos.N());
    if (terrain >= MINE_START && terrain <= MINE_END) {
      addItem(pos.N());
    }
    terrain = mapGetPos(mp, pos.S());
    if (terrain >= MINE_START && terrain <= MINE_END) {
      addItem(pos.S());
    }
    terrain = mapGetPos(mp, pos.W());
    if (terrain >= MINE_START && terrain <= MINE_END) {
      addItem(pos.W());
    }
    terrain = mapGetPos(mp, pos.E());
    if (terrain >= MINE_START && terrain <= MINE_END) {
      addItem(pos.E());
    }

    screenReCalc();
  }
}

}  // namespace bolo
