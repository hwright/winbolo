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

// Linux Frontend implementation

#ifndef _LINUX_FRONTEND_H
#define _LINUX_FRONTEND_H

#include "../../bolo/frontend.h"

namespace bolo {

class LinuxFrontend : public Frontend {
 public:
  void updateTankSupplyBars(TankSupply tank_supply) override;
  void updateBaseSupplyBars(BaseSupply base_supply) override;
  void playSound(sndEffects value) override;
  void drawMainScreen(ScreenTiles tiles, ScreenTankList tks,
                      std::optional<ScreenGunsight> gunsight,
                      ScreenBulletList sBullet, ScreenLgmList lgms,
                      long srtDelay, bool isPillView, int edgeX,
                      int edgeY) override;
  void statusPillbox(uint8_t pillNum, pillAlliance pb) override;
  void statusTank(uint8_t tankNum, tankAlliance ts) override;
  void statusBase(uint8_t baseNum, baseAlliance bs) override;
  void messages(std::string_view top, std::string_view bottom) override;
  void killsDeaths(int kills, int deaths) override;
  void setManStatus(std::optional<ManStatus> status) override;
  void gameOver(void) override;
  void clearPlayer(playerNumbers value) override;
  void setPlayer(playerNumbers value, std::string_view str) override;
  void drawDownload(bool justBlack) override;
  void selectIndent(buildSelect old_val, buildSelect new_val) override;
  void setPlayerCheckState(playerNumbers value, bool isChecked) override;
  void enableRequestAllyMenu(bool enabled) override;
  void enableLeaveAllyMenu(bool enabled) override;
  void showGunsight(bool isShown) override;
  void redrawAll(void) override {}
  bool tutorial(BYTE pos) override;

 private:
  TankSupply tank_supply_;
  BaseSupply base_supply_;
};

}  // namespace bolo

#endif
