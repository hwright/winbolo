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
 *Name:          Flood Fill
 *Filename:      floodfill.h
 *Author:        John Morrison
 *Creation Date: 19/1/99
 *Last Modified: 21/1/99
 *Purpose:
 *  Responsible for tracking flood fills caused by craters
 *  next to water.
 *********************************************************/

#ifndef FLOOD_H
#define FLOOD_H

#include <unordered_map>

#include "global.h"
#include "types.h"

namespace bolo {

class FloodFill {
 public:
  FloodFill() = default;

  // Move-only
  FloodFill(FloodFill &) = delete;
  FloodFill &operator=(FloodFill &) = delete;

  // Add a flood item.
  //
  // ARGUMENTS:
  //  pos - the flood position
  void addItem(MapPoint pos);

  // Game tick has happened. Update flooding
  //
  // ARGUMENTS:
  //  mp - Pointer to the map structure
  //  pb - Pointer to the pillboxes structure
  //  bs - Pointer to the bases structure
  void Update(map *mp, pillboxes *pb, bases *bs);

 private:
  // Time to fill if required. Also if it does adds
  // surrounding items to flood Data Structure.
  //
  // ARGUMENTS:
  //  mp - Pointer to the map structure
  //  pb - Pointer to the pillboxes structure
  //  bs - Pointer to the bases structure
  //  pos - The flood position.
  void checkFill(map *mp, pillboxes *pb, bases *bs, MapPoint pos);

  std::unordered_map<MapPoint, uint8_t> floods_;
};

}  // namespace bolo

#endif /* FLOOD_H */
