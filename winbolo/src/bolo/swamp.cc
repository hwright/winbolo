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

#include "swamp.h"

#include "global.h"

namespace bolo {

namespace {

// Maximum life.  Gives 4 shots to death.
const int LIFE = 3;

// Shells die when there length equals
const int DEATH = 0;

/* What grass truns into when it dies */
const BYTE DEATH_RETURN = RIVER;

}  // namespace

BYTE SwampState::addItem(MapPoint pos) {
  if (auto it = swamps_.find(pos); it != swamps_.end()) {
    it->second -= 1;
    if (it->second == DEATH) {
      swamps_.erase(it);
      return DEATH_RETURN;
    }
  } else {
    swamps_[pos] = LIFE;
  }

  return SWAMP;
}

void SwampState::removePos(MapPoint pos) { swamps_.erase(pos); }

}  // namespace bolo
