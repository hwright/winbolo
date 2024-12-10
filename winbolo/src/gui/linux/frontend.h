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
#include "draw.h"

namespace bolo {

class LinuxFrontend : public Frontend {
 public:
  // TODO: Parameterize this by the number of bases.
  LinuxFrontend();

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
  void drawAll(void) override;
  bool tutorial(BYTE pos) override;
  void print_error(std::string_view) override;

 private:
  TankSupply tank_supply_;
  BaseSupply base_supply_;

  std::vector<baseAlliance> base_status_;
  std::vector<tankAlliance> tank_status_;
  std::vector<pillAlliance> pill_status_;

  std::optional<MainScreenData> main_screen_data_;

  int kills_ = 0;
  int deaths_ = 0;

  std::string top_message_;
  std::string bottom_message_;

  std::optional<ManStatus> man_status_;
  buildSelect build_select_;
};

}  // namespace bolo

#endif
