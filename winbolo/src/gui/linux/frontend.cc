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
#include "framemutex.h"
#include "sound.h"

extern uint64_t dwSysFrame;
extern bool soundEffects;
extern bool showPillLabels;
extern bool showBaseLabels;
extern SDL_TimerID timerGameID;
extern SDL_TimerID timerFrameID;
extern char messageBody[16 * 1024];
extern char messageTitle[256];
extern int frameRateTime;
extern uint8_t numMessages;
extern bool isInMenu;
extern bool hideMainView;

extern GtkWidget *idc_player1;
extern GtkWidget *idc_player2;
extern GtkWidget *idc_player3;
extern GtkWidget *idc_player4;
extern GtkWidget *idc_player5;
extern GtkWidget *idc_player6;
extern GtkWidget *idc_player7;
extern GtkWidget *idc_player8;
extern GtkWidget *idc_player9;
extern GtkWidget *idc_player10;
extern GtkWidget *idc_player11;
extern GtkWidget *idc_player12;
extern GtkWidget *idc_player13;
extern GtkWidget *idc_player14;
extern GtkWidget *idc_player15;
extern GtkWidget *idc_player16;
extern GtkWidget *request_alliance1;
extern GtkWidget *leave_alliance1;

namespace bolo {

namespace {

const char *STR_01 = "1";
const char *STR_02 = "2";
const char *STR_03 = "3";
const char *STR_04 = "4";
const char *STR_05 = "5";
const char *STR_06 = "6";
const char *STR_07 = "7";
const char *STR_08 = "8";
const char *STR_09 = "9";
const char *STR_10 = "10";
const char *STR_11 = "11";
const char *STR_12 = "12";
const char *STR_13 = "13";
const char *STR_14 = "14";
const char *STR_15 = "15";
const char *STR_16 = "16";

}  // namespace

void LinuxFrontend::updateTankStatusBars(uint8_t shells, uint8_t mines,
                                         uint8_t armour, uint8_t trees) {
  uint64_t tick;

  tick = SDL_GetTicks();
  drawStatusTankBars(0, 0, shells, mines, armour, trees);
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::updateBaseStatusBars(uint8_t shells, uint8_t mines,
                                         uint8_t armour) {
  uint64_t tick = SDL_GetTicks();
  drawStatusBaseBars(0, 0, shells, mines, armour, FALSE);
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::playSound(sndEffects value) {
  if (soundEffects) {
    soundPlayEffect(value);
  }
}

void LinuxFrontend::drawMainScreen(screen *value, screenMines *mineView,
                                   screenTanks *tks, screenGunsight *gs,
                                   screenBullets *sBullet, screenLgm *lgms,
                                   long srtDelay, bool isPillView, tank *tank,
                                   int edgeX, int edgeY) {
  uint8_t cursorX;
  uint8_t cursorY;
  bool showCursor;

  showCursor = screenGetCursorPos(&cursorX, &cursorY);
  ::drawMainScreen(value, mineView, tks, gs, sBullet, lgms, showPillLabels,
                   showBaseLabels, srtDelay, isPillView, edgeX, edgeY,
                   showCursor, cursorX, cursorY);
}

void LinuxFrontend::statusPillbox(uint8_t pillNum, pillAlliance pb) {
  uint64_t tick = SDL_GetTicks();
  drawStatusPillbox(pillNum, pb, showPillLabels);
  drawCopyPillsStatus();
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::statusTank(uint8_t tankNum, tankAlliance ts) {
  uint64_t tick = SDL_GetTicks();
  drawStatusTank(tankNum, ts);
  drawCopyTanksStatus();
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::statusBase(uint8_t baseNum, baseAlliance bs) {
  uint64_t tick = SDL_GetTicks();
  drawStatusBase(baseNum, bs, showBaseLabels);
  drawCopyBasesStatus();
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::messages(std::string_view top, std::string_view bottom) {
  uint64_t tick = SDL_GetTicks();
  frameMutexWaitFor();
  drawMessages(0, 0, std::string(top).c_str(), std::string(bottom).c_str());
  frameMutexRelease();
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::killsDeaths(int kills, int deaths) {
  uint64_t tick = SDL_GetTicks();
  drawKillsDeaths(0, 0, kills, deaths);
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::manStatus(bool isDead, TURNTYPE angle) {
  uint64_t tick = SDL_GetTicks();
  frameMutexWaitFor();
  drawSetManStatus(isDead, angle, TRUE);
  frameMutexRelease();
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::manClear(void) { drawSetManClear(); }

void LinuxFrontend::gameOver(void) {
  frameRateTime = 0;
  SDL_RemoveTimer(timerGameID);
  SDL_RemoveTimer(timerFrameID);
  strcpy(messageTitle, "LinBolo");
  strcpy(messageBody, langGetText(STR_WBTIMELIMIT_END));
  numMessages = 1;
}

void LinuxFrontend::clearPlayer(playerNumbers value) {
  isInMenu = true;
  switch (value) {
    case player01:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player1)->child)), STR_01);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player1), FALSE);
      gtk_widget_set_sensitive(idc_player1, FALSE);
      break;
    case player02:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player2)->child)), STR_02);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player2), FALSE);
      gtk_widget_set_sensitive(idc_player2, FALSE);
      break;
    case player03:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player3)->child)), STR_03);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player3), FALSE);
      gtk_widget_set_sensitive(idc_player3, FALSE);
      break;
    case player04:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player4)->child)), STR_04);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player4), FALSE);
      gtk_widget_set_sensitive(idc_player4, FALSE);
      break;
    case player05:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player5)->child)), STR_05);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player5), FALSE);
      gtk_widget_set_sensitive(idc_player5, FALSE);
      break;
    case player06:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player6)->child)), STR_06);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player6), FALSE);
      gtk_widget_set_sensitive(idc_player6, FALSE);
      break;
    case player07:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player7)->child)), STR_07);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player7), FALSE);
      gtk_widget_set_sensitive(idc_player7, FALSE);
      break;
    case player08:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player8)->child)), STR_08);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player8), FALSE);
      gtk_widget_set_sensitive(idc_player8, FALSE);
      break;
    case player09:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player9)->child)), STR_09);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player9), FALSE);
      gtk_widget_set_sensitive(idc_player9, FALSE);
      break;
    case player10:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player10)->child)), STR_10);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player10), FALSE);
      gtk_widget_set_sensitive(idc_player10, FALSE);
      break;
    case player11:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player11)->child)), STR_11);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player11), FALSE);
      gtk_widget_set_sensitive(idc_player11, FALSE);
      break;
    case player12:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player12)->child)), STR_12);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player12), FALSE);
      gtk_widget_set_sensitive(idc_player12, FALSE);
      break;
    case player13:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player13)->child)), STR_13);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player13), FALSE);
      gtk_widget_set_sensitive(idc_player13, FALSE);
      break;
    case player14:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player14)->child)), STR_14);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player14), FALSE);
      gtk_widget_set_sensitive(idc_player14, FALSE);
      break;
    case player15:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player15)->child)), STR_15);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player15), FALSE);
      gtk_widget_set_sensitive(idc_player15, FALSE);
      break;
    case player16:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player16)->child)), STR_16);
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player16), FALSE);
      gtk_widget_set_sensitive(idc_player16, FALSE);
      break;
  }
  isInMenu = false;
}

void LinuxFrontend::setPlayer(playerNumbers value, std::string_view str) {
  isInMenu = true;
  switch (value) {
    case player01:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player1)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player1, TRUE);
      break;
    case player02:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player2)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player2, TRUE);
      break;
    case player03:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player3)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player3, TRUE);
      break;
    case player04:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player4)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player4, TRUE);
      break;
    case player05:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player5)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player5, TRUE);
      break;
    case player06:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player6)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player6, TRUE);
      break;
    case player07:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player7)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player7, TRUE);
      break;
    case player08:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player8)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player8, TRUE);
      break;
    case player09:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player9)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player9, TRUE);
      break;
    case player10:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player10)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player10, TRUE);
      break;
    case player11:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player11)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player11, TRUE);
      break;
    case player12:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player12)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player12, TRUE);
      break;
    case player13:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player13)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player13, TRUE);
      break;
    case player14:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player14)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player14, TRUE);
      break;
    case player15:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player15)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player15, TRUE);
      break;
    case player16:
      gtk_label_set_text((GTK_LABEL(GTK_BIN(idc_player16)->child)),
                         std::string(str).c_str());
      gtk_widget_set_sensitive(idc_player16, TRUE);
      break;
  }
  isInMenu = false;
}

void LinuxFrontend::drawDownload(bool justBlack) {
  if (!hideMainView) {
    uint64_t tick = SDL_GetTicks();
    frameMutexWaitFor();
    drawDownloadScreen(justBlack);
    frameMutexRelease();
    dwSysFrame += (SDL_GetTicks() - tick);
  }
}

void LinuxFrontend::setPlayerCheckState(playerNumbers value, bool isChecked) {
  isInMenu = true;
  switch (value) {
    case player01:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player1),
                                     isChecked);
      break;
    case player02:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player2),
                                     isChecked);
      break;
    case player03:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player3),
                                     isChecked);
      break;
    case player04:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player4),
                                     isChecked);
      break;
    case player05:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player5),
                                     isChecked);
      break;
    case player06:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player6),
                                     isChecked);
      break;
    case player07:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player7),
                                     isChecked);
      break;
    case player08:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player8),
                                     isChecked);
      break;
    case player09:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player9),
                                     isChecked);
      break;
    case player10:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player10),
                                     isChecked);
      break;
    case player11:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player11),
                                     isChecked);
      break;
    case player12:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player12),
                                     isChecked);
      break;
    case player13:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player13),
                                     isChecked);
      break;
    case player14:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player14),
                                     isChecked);
      break;
    case player15:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player15),
                                     isChecked);
      break;
    default:
      /* case player16: */
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(idc_player16),
                                     isChecked);
      break;
  }
  isInMenu = false;
}

void LinuxFrontend::enableRequestAllyMenu(bool enabled) {
  gtk_widget_set_sensitive(request_alliance1, enabled);
}

void LinuxFrontend::enableLeaveAllyMenu(bool enabled) {
  gtk_widget_set_sensitive(leave_alliance1, enabled);
}

}  // namespace bolo
