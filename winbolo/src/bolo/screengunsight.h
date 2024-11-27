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

// Defines the gunsight position on the screen

#ifndef _SCREENGUNSIGHT_H
#define _SCREENGUNSIGHT_H

#include "types.h"

namespace bolo {

struct ScreenGunsight {
  MapPoint pos = {.x = 0, .y = 0};
  uint8_t pixelX = 0;
  uint8_t pixelY = 0;
};

}  // namespace bolo

#endif
