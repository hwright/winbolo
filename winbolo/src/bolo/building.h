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

//  Responsible for tracking lifetime of Buildings.
//  buildings can be shot 5 times before being destroyed

#ifndef BUILDING_H
#define BUILDING_H

#include <unordered_map>

#include "global.h"

namespace bolo {

class BuildingState {
 public:
  BuildingState() = default;

  // Move-only
  BuildingState(BuildingState &) = delete;
  BuildingState &operator=(BuildingState &) = delete;

  // Add a building item. If it already exists return
  // the terrain type of the item and decrements its lifetime.
  //
  // ARGUMENTS:
  //  pos - The building position
  BYTE addItem(MapPoint pos);

  // Remove a building item if it exists at a specific
  // location. Otherwise the function does nothing.
  //
  // ARGUMENTS:
  //  pos - The building position
  void removePos(MapPoint pos) { buildings_.erase(pos); }

 private:
  std::unordered_map<MapPoint, uint8_t> buildings_;
};

}  // namespace bolo

#endif /* BUILDING_H */
