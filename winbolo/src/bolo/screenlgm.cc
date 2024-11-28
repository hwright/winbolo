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
 *Name:          Screen LGM
 *Filename:      screenLgm.c
 *Author:        John Morrison
 *Creation Date: 19/2/99
 *Last Modified: 26/11/99
 *Purpose:
 *  Responsable for Lgms on the screen
 *********************************************************/

#include "screenlgm.h"

#include "backend.h"
#include "global.h"
#include "lgm.h"
#include "players.h"
#include "screen.h"

namespace bolo {

void ScreenLgmList::prepare(uint8_t leftPos, uint8_t rightPos, uint8_t top,
                            uint8_t bottom) {
  uint8_t mx; /* THIS lgm if on screen */
  uint8_t my;
  uint8_t px;
  uint8_t py;
  uint8_t frame;

  if (lgmOnScreen(screenGetLgmFromPlayerNum(playersGetSelf(screenGetPlayers())),
                  leftPos, rightPos, top, bottom)) {
    lgmGetScreenCoords(
        screenGetLgmFromPlayerNum(playersGetSelf(screenGetPlayers())), leftPos,
        top, &mx, &my, &px, &py, &frame);
    lgms_.push_back(ScreenLgm{
        .pos = MapPoint{.x = mx, .y = my}, .px = px, .py = py, .frame = frame});
  }
  playersMakeScreenLgm(screenGetPlayers(), this, leftPos, rightPos, top,
                       bottom);
}

}  // namespace bolo
