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
 *Name:          Rubble
 *Filename:      rubble.c
 *Author:        John Morrison
 *Creation Date: 30/12/98
 *Last Modified: 25/04/01
 *Purpose:
 *  Responsable for tracking lifetime of rubble.
 *  buildings can be shot 5 times before being destroyed
 *********************************************************/

#include "rubble.h"

#include "global.h"

BYTE RubbleState::addItem(MapPoint pos) {
  if (auto it = rubbles_.find(pos); it != rubbles_.end()) {
    // Decrement the life count and check for death.
    it->second -= 1;
    if (it->second == RUBBLE_DEATH) {
      rubbles_.erase(it);
      return RIVER;
    }
  } else {
    // Insert a new rubble
    rubbles_[pos] = RUBBLE_LIFE;
  }

  return RUBBLE;
}

void RubbleState::removePos(MapPoint pos) { rubbles_.erase(pos); }
