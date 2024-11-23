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
 
// Responsible for tracking mine explosions, chain
// reactions and damage to tank from them.

#ifndef MINEEXP_H
#define MINEEXP_H

#include "global.h"
#include "lgm.h"
#include "types.h"

namespace bolo {

class MineExplosionTracker {
 public:
  MineExplosionTracker() = default;

  // Move-only
  MineExplosionTracker(MineExplosionTracker &) = delete;
  MineExplosionTracker &operator=(MineExplosionTracker &) = delete;

  // Adds an item to the minesExp data structure.
  //
  // ARGUMENTS:
  //  mp -  Map Structure
  //  pos - The mine position
  void addItem(MapPoint pos);

  // Game tick has happened. Update mine explosions.
  //
  // ARGUMENTS:
  //  mp     - Pointer to the map structure
  //  pb     - Pointer to the pillboxes structure
  //  bs     - Pointer to the bases structure
  //  lgms   - Array of lgms
  //  numLgm - Number of lgms in the array
  void Update(map *mp, pillboxes *pb, bases *bs, lgm **lgms, BYTE numLgm);

 private:
  // Time to fill if required. Also if it does adds
  // surrounding items to minesExp Data Structure.
  //
  // ARGUMENTS:
  //  mp     - Pointer to the map structure
  //  pb     - Pointer to the pillboxes structure
  //  bs     - Pointer to the bases structure
  //  lgms   - Array of lgms
  //  numLgm - Number of lgms in the array
  //  pos    - The mine position
  void checkFill(map *mp, pillboxes *pb, bases *bs, lgm **lgms, BYTE numLgm,
                 MapPoint pos);

  std::unordered_map<MapPoint, uint8_t> explosions_;
};

}  // namespace bolo

#endif /* MINEEXP_H */
