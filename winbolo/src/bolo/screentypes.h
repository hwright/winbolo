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

// Various types which represent computed values before to display.

#ifndef _SCREENTYPES_H
#define _SCREENTYPES_H

#include <vector>

#include "types.h"

namespace bolo {

struct ScreenGunsight {
  MapPoint pos = {.x = 0, .y = 0};
  uint8_t pixelX = 0;
  uint8_t pixelY = 0;
};

struct ScreenBullet {
  // Map point of the bullet (mapped to screen)
  MapPoint pos;
  uint8_t px;     // X Pixel offset of the bullet
  uint8_t py;     // Y Pixel offset of the bullet
  uint8_t frame;  // Frame identifier type
};

using ScreenBulletList = std::vector<ScreenBullet>;

}  // namespace bolo

#endif
