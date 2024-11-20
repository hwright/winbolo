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
 *Filename:      rubble.h
 *Author:        John Morrison
 *Creation Date: 30/12/98
 *Last Modified: 30/12/98
 *Purpose:
 *  Responsable for tracking lifetime of rubble.
 *  buildings can be shot 5 times before being destroyed
 *********************************************************/

#ifndef RUBBLE_H
#define RUBBLE_H

#include <unordered_map>

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) == NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define RubbleHeadX(list) ((list)->x);
#define RubbleHeadY(list) ((list)->y);
#define RubbleHeadLength(list) ((list)->life);
#define RubbleTail(list) ((list)->next);

/* How manu shots it takes to destroy a building */
#define RUBBLE_LIFE 4

/* Shells die when there length equals */
#define RUBBLE_DEATH 0

/* Type structure */

class RubbleState {
 public:
  // Add an item of rubble.
  // If one already exists at the location, return the terrain
  // type of the item and decrements its lifetime.
  //
  // ARGUMENTS:
  //  pos - The rubble position
  BYTE addItem(MapPoint pos);

  // Remove an item of rubble if it exists at a specific
  // location.
  //
  // ARGUMENTS:
  //  pos - The rubble position
  void removePos(MapPoint pos);

 private:
  // A map from points to their lives.
  std::unordered_map<MapPoint, uint8_t> rubbles_;
};

#endif /* RUBBLE_H */
