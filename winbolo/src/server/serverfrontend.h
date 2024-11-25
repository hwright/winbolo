/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024-     Hyrum Wright
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

// Provides a dummy Frontend implementation for the server.

#include "../bolo/frontend.h"

namespace bolo {

class ServerFrontend : public Frontend {
 public:
  void updateTankStatusBars(uint8_t shells, uint8_t mines, uint8_t armour,
                            uint8_t trees) override {}

  void updateBaseStatusBars(uint8_t shells, uint8_t mines,
                            uint8_t armour) override {}

  void playSound(sndEffects value) override {}

  void drawMainScreen(screen *value, screenMines *mineView, screenTanks *tks,
                      screenGunsight *gs, screenBullets *sBullet,
                      screenLgm *lgms, long srtDelay, bool isPillView,
                      tank *tank, int edgeX, int edgeY) override {}

  void statusPillbox(uint8_t pillNum, pillAlliance pb) override {}
  void statusTank(uint8_t tankNum, tankAlliance ts) override {}
  void statusBase(uint8_t baseNum, baseAlliance bs) override {}
  void messages(std::string_view top, std::string_view bottom) override {}
  void killsDeaths(int kills, int deaths) override {}
  void manStatus(bool isDead, TURNTYPE angle) override {}
  void manClear(void) override {}
  void gameOver(void) override;
  void clearPlayer(playerNumbers value) override {}
  void setPlayer(playerNumbers value, std::string_view str) override {}
};

}  // namespace bolo
