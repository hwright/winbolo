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

#include "screentypes.h"

#include "../gui/lang.h"
#include "backend.h"
#include "global.h"
#include "labels.h"
#include "lgm.h"
#include "players.h"
#include "screen.h"
#ifdef _WIN32
#include "../gui/resource.h"
#else
#include "../gui/linresource.h"
#endif

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

void ScreenTankList::prepare(tank *tnk, uint8_t leftPos, uint8_t rightPos,
                             uint8_t top, uint8_t bottom) {
  uint8_t x; /* X and Y map pos of it */
  uint8_t y;

  numTanksScreen = 0;

  x = tankGetScreenMX(tnk);
  y = tankGetScreenMY(tnk);

  if (x >= leftPos && x <= rightPos && y >= top && y <= bottom) {
    numTanksScreen = 1;
    std::string label;

    // Get the tank's names
    if (tankGetArmour(tnk) <= TANK_FULL_ARMOUR) {
      char playerName[PLAYER_NAME_LEN] = "\0";  // Player Name
      playersGetPlayerName(screenGetPlayers(),
                           playersGetSelf(screenGetPlayers()), playerName);
      label = labelMakeTankLabel(playerName, langGetText(MESSAGE_THIS_COMPUTER),
                                 true);
    }

    tanks.emplace_back(
        ScreenTank{.pos = MapPoint{.x = static_cast<uint8_t>(x - leftPos),
                                   .y = static_cast<uint8_t>(y - top)},
                   .px = tankGetScreenPX(tnk),
                   .py = tankGetScreenPY(tnk),
                   .frame = tankGetFrame(tnk),
                   .playerNum = playersGetSelf(screenGetPlayers()),
                   .playerName = label});
  }

  // Add the rest of the tanks as required
  playersMakeScreenTanks(screenGetPlayers(), this, leftPos, rightPos, top,
                         bottom);
}

}  // namespace bolo
