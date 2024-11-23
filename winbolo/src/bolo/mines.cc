/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024      Hyrum Wright.
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

#include "mines.h"

#include "bolo_map.h"
#include "global.h"
#include "screen.h"

bool MineTracker::addItem(MapPoint pos) {
  // bitset doesn't play nicely with std::exchange.
  bool returnValue = mines_[pos.x][pos.y];
  mines_[pos.x][pos.y] = true;
  return returnValue;
}

bool MineTracker::existPos(MapPoint pos) {
  bool returnValue = true; /* Value to return */

  if (pos.x <= MAP_MINE_EDGE_LEFT || pos.x >= MAP_MINE_EDGE_RIGHT ||
      pos.y <= MAP_MINE_EDGE_TOP || pos.y >= MAP_MINE_EDGE_BOTTOM) {
    returnValue = true;
  } else if (allowHidden_) {
    returnValue = mines_[pos.x][pos.y];
  } else {
    returnValue = screenMapIsMine(pos.x, pos.y);
  }
  return returnValue;
}
