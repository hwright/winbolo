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

#include "building.h"

#include "global.h"

namespace bolo {

namespace {

/* How manu shots it takes to destroy a building */
const int LIFE = 4;

/* Shells die when there length equals */
const int DEATH = 0;

}  // namespace

BYTE BuildingState::addItem(MapPoint pos) {
  if (auto it = buildings_.find(pos); it != buildings_.end()) {
    it->second -= 1;
    if (it->second == DEATH) {
      buildings_.erase(it);
      return RUBBLE;
    }
  } else {
    buildings_.insert({pos, LIFE});
  }

  return HALFBUILDING;
}

}  // namespace bolo
