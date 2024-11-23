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
 *Name:          Mines
 *Filename:      mines.h
 *Author:        John Morrison
 *Creation Date: 29/1/99
 *Last Modified:  8/2/99
 *Purpose:
 *  Handles what mines are visible to the user
 *********************************************************/

#ifndef MINES_H
#define MINES_H

/* Defines */
#include <array>
#include <bitset>

#include "global.h"

#define MINES_ARRAY_SIZE 256 /* maps are 256x256 units square */

class MineTracker {
 public:
  explicit MineTracker(bool allowHidden) : allowHidden_(allowHidden) {}

  // Move-only
  MineTracker(MineTracker &) = delete;
  MineTracker &operator=(MineTracker &) = delete;

  // Returns whether hidden mines are allowed or not
  bool allowHidden() { return allowHidden_; }

  // Add a mine. Returns whether a mine already existed
  // at that position.
  //
  // ARGUMENTS:
  //  pos - The mine position.
  bool addItem(MapPoint pos);

  // Remove a mine.
  //
  // ARGUMENTS:
  //  pos - The mine position.
  void removeItem(MapPoint pos);

  // Return whether a mine can be seen at that position.
  // Only called if a mine does exist at the map square
  // but checking here to see if the player knows about it
  //
  // ARGUMENTS:
  //  pos - The mine position.
  bool existPos(MapPoint pos);

 private:
  std::array<std::bitset<MINES_ARRAY_SIZE>, MINES_ARRAY_SIZE> mines_;

  // Are hidden mines allowed?
  bool allowHidden_;
};

#endif /* MINES_H */
