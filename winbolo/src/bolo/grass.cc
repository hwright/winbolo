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

#include "grass.h"

#include "global.h"

namespace bolo {

namespace {

// How manu shots it takes to destroy a piece of grass
const int LIFE = 4;

// Shells die when there length equals
const int DEATH = 0;

// What grass truns into when it dies
const BYTE DEATH_RETURN = SWAMP;

}  // namespace

BYTE GrassState::addItem(MapPoint pos) {
  if (auto it = grasses_.find(pos); it != grasses_.end()) {
    it->second -= 1;
    if (it->second == DEATH) {
      grasses_.erase(it);
      return DEATH_RETURN;
    }
  } else {
    grasses_[pos] = LIFE;
  }

  return GRASS;
}

}  // namespace bolo
