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

// Responsable for tracking lifetime of grass when shot
// from a boat

#ifndef GRASS_H
#define GRASS_H

#include <unordered_map>

#include "global.h"

namespace bolo {

class GrassState {
 public:
  GrassState() = default;

  // Move-only
  GrassState(GrassState &) = delete;
  GrassState &operator=(GrassState &) = delete;

  // Add an item to the grass data structure.
  // If it already exists returns the terrain type of the
  // item and decrements its lifetime.
  //
  // ARGUMENTS:
  //  pos - The position of the object.
  BYTE addItem(MapPoint pos);

  // Remove an item from the grass data structure if it
  // exists at a specific loaction. Otherwise the function
  // does nothing.
  //
  // ARGUMENTS:
  //  pos - The position of the object.
  void removePos(MapPoint pos) { grasses_.erase(pos); }

 private:
  std::unordered_map<MapPoint, uint8_t> grasses_;
};

}  // namespace bolo

#endif /* GRASS_H */
