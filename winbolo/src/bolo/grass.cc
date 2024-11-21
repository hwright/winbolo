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
 *Name:          Grass
 *Filename:      grass.c
 *Author:        John Morrison
 *Creation Date: 5/1/99
 *Last Modified: 25/04/01
 *Purpose:
 *  Responsable for tracking lifetime of grass when shot
 *  from a boat
 *********************************************************/

#include "grass.h"

#include "global.h"

namespace {

// How manu shots it takes to destroy a piece of grass
const int LIFE = 4;

// Shells die when there length equals
const int DEATH = 0;

// What grass truns into when it dies
const BYTE DEATH_RETURN = SWAMP;

}

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
