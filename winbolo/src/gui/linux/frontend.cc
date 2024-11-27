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
#include <gtk/gtk.h>

#include "../clientmutex.h"
#include "../lang.h"
#include "../linresource.h"
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
extern char messageBody2[16 * 1024];
extern char messageTitle2[256];
extern char messageBody3[16 * 1024];
extern char messageTitle3[256];
extern uint8_t numMessages;
extern bool isInMenu;
extern bool hideMainView;
extern bool isTutorial;
extern BYTE upTo;
extern bool doingTutorial;

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
extern GtkWidget *show_gunsight1;

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

const char *DIALOG_BOX_TITLE = "LinBolo";

}  // namespace

void LinuxFrontend::updateTankSupplyBars(TankSupply tank_supply) {
  tank_supply_ = std::move(tank_supply);
  uint64_t tick;

  tick = SDL_GetTicks();
  drawStatusTankBars(0, 0, tank_supply_.shells, tank_supply_.mines,
                     tank_supply_.armor, tank_supply_.trees);
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::updateBaseSupplyBars(BaseSupply base_supply) {
  base_supply_ = std::move(base_supply);
  uint64_t tick = SDL_GetTicks();
  drawStatusBaseBars(0, 0, base_supply_.shells, base_supply_.mines,
                     base_supply_.armor, false);
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::playSound(sndEffects value) {
  if (soundEffects) {
    soundPlayEffect(value);
  }
}

void LinuxFrontend::drawMainScreen(ScreenTiles tiles, ScreenTankList tks,
                                   std::optional<ScreenGunsight> gunsight,
                                   ScreenBulletList sBullet, ScreenLgmList lgms,
                                   long srtDelay, bool isPillView, int edgeX,
                                   int edgeY) {
  uint8_t cursorX;
  uint8_t cursorY;
  bool showCursor;

  showCursor = screenGetCursorPos(&cursorX, &cursorY);
  ::drawMainScreen(tiles, tks, std::move(gunsight), sBullet, lgms,
                   showPillLabels, showBaseLabels, srtDelay, isPillView, edgeX,
                   edgeY, showCursor, cursorX, cursorY);
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

void LinuxFrontend::setManStatus(std::optional<ManStatus> status) {
  if (status.has_value()) {
    uint64_t tick = SDL_GetTicks();
    frameMutexWaitFor();
    drawSetManStatus(status->is_dead, status->angle, TRUE);
    frameMutexRelease();
    dwSysFrame += (SDL_GetTicks() - tick);
  } else {
    drawSetManClear();
  }
}

void LinuxFrontend::gameOver(void) {
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

void LinuxFrontend::selectIndent(buildSelect old_val, buildSelect new_val) {
  drawSelectIndentsOff(old_val, 0, 0);
  drawSelectIndentsOn(new_val, 0, 0);
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

void LinuxFrontend::showGunsight(bool isShown) {
  isInMenu = true;
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(show_gunsight1), isShown);
  isInMenu = false;
}

bool LinuxFrontend::tutorial(BYTE pos) {
  bool returnValue; /* Value to return */

  // gdk_threads_enter();
  returnValue = false;
  if (isTutorial) {
    switch (upTo) {
      case 0:
        if (pos == 208) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL01));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 1:
        if (pos == 197) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL02));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 2:
        if (pos == 192) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL03));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 3:
        if (pos == 186) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL04));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 4:
        if (pos == 181) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL05));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 5:
        if (pos == 175) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, "LinBolo");
          strcpy(messageBody, langGetText(STR_TUTORIAL06));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 6:
        if (pos == 166) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL07));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 7:
        if (pos == 159) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL09));
          strcpy(messageTitle2, DIALOG_BOX_TITLE);
          strcpy(messageBody2, langGetText(STR_TUTORIAL08));
          numMessages = 2;
          returnValue = true;
          upTo++;
          upTo++;
        }
        break;
      case 8:
        upTo++;
        break;
      case 9:
        if (pos == 142) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL11));
          strcpy(messageTitle2, DIALOG_BOX_TITLE);
          strcpy(messageBody2, langGetText(STR_TUTORIAL10));
          numMessages = 2;
          returnValue = true;
          upTo++;
          upTo++;
        }
        break;
      case 10:
        upTo++;
        break;
      case 11:
        if (pos == 122) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL12));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 12:
        if (pos == 120) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL14));
          strcpy(messageTitle2, DIALOG_BOX_TITLE);
          strcpy(messageBody2, langGetText(STR_TUTORIAL13));
          numMessages = 2;
          returnValue = true;
          upTo++;
          upTo++;
        }
        break;
      case 13:
        upTo++;
        break;
      case 14:
        if (pos == 110) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL15));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 15:
        if (pos == 103) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL16));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 16:
        if (pos == 98) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL17));
          numMessages = 1;
          returnValue = true;
          upTo++;
        }
        break;
      case 17:
        if (pos == 84) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL19));
          strcpy(messageTitle2, DIALOG_BOX_TITLE);
          strcpy(messageBody2, langGetText(STR_TUTORIAL18));
          numMessages = 2;
          returnValue = true;
          upTo++;
          upTo++;
        }
        break;
      case 18:
        upTo++;
        break;
      case 19:
        if (pos == 66) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL21));
          strcpy(messageTitle2, DIALOG_BOX_TITLE);
          strcpy(messageBody2, langGetText(STR_TUTORIAL20));
          numMessages = 2;
          returnValue = true;
          upTo++;
          upTo++;
        }
        break;
      case 20:
        upTo++;
        break;
      case 21:
        if (pos == 47) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle3, DIALOG_BOX_TITLE);
          strcpy(messageBody3, langGetText(STR_TUTORIAL22));
          strcpy(messageTitle2, DIALOG_BOX_TITLE);
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody2, langGetText(STR_TUTORIAL23));
          strcpy(messageBody, langGetText(STR_TUTORIAL24));
          numMessages = 3;
          returnValue = true;
          upTo++;
          upTo++;
        }
        break;
      case 22:
        upTo++;
        break;
      case 23:
        upTo++;
        break;
      case 24:
        if (pos == 21) {
          doingTutorial = true;
          clientMutexRelease();
          strcpy(messageTitle, DIALOG_BOX_TITLE);
          strcpy(messageBody, langGetText(STR_TUTORIAL25));
          numMessages = 1;
          returnValue = true;
          upTo++;
          SDL_RemoveTimer(timerGameID);
        }
        break;
      default:
        break;
    }
  }

  return returnValue;
}

}  // namespace bolo
