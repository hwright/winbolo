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

//  Responsible for tracking lifetime of rubble.
//  Buildings can be shot 5 times before being destroyed

#ifndef RUBBLE_H
#define RUBBLE_H

#include <unordered_map>

#include "global.h"

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
