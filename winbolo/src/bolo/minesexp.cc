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

void MineExplosionTracker::addItem(MapPoint pos) {
  if (!explosions_.contains(pos)) {
    explosions_.insert({pos, MINES_EXPLOSION_WAIT});
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
  bool abovePos; /* Are squares around mines? */
  bool belowPos;
  bool leftPos;
  bool rightPos;

  abovePos = false;
  belowPos = false;
  leftPos = false;
  rightPos = false;
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
      abovePos = true;
    }
    terrain = mapGetPos(mp, pos.S());
    if (terrain >= MINE_START && terrain <= MINE_END) {
      belowPos = true;
    }
    terrain = mapGetPos(mp, pos.W());
    if (terrain >= MINE_START && terrain <= MINE_END) {
      leftPos = true;
    }
    terrain = mapGetPos(mp, pos.E());
    if (terrain >= MINE_START && terrain <= MINE_END) {
      rightPos = true;
    }

    /* Add items if craters */
    if (leftPos) {
      addItem(pos.W());
    }
    if (rightPos) {
      addItem(pos.E());
    }
    if (abovePos) {
      addItem(pos.N());
    }
    if (belowPos) {
      addItem(pos.S());
    }
    screenReCalc();
  }
}
