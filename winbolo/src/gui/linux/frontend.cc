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

#include "frontend.h"

#include <SDL.h>

#include "draw.h"

extern uint64_t dwSysFrame;

namespace bolo {

void LinuxFrontend::updateTankStatusBars(BYTE shells, BYTE mines, BYTE armour,
                                         BYTE trees) {
  uint64_t tick;

  tick = SDL_GetTicks();
  drawStatusTankBars(0, 0, shells, mines, armour, trees);
  dwSysFrame += (SDL_GetTicks() - tick);
}

}  // namespace bolo
