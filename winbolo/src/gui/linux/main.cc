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

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <format>
#include <fstream>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <sstream>

#include "../../bolo/backend.h"
#include "../../bolo/global.h"
#include "../../bolo/network.h"
#include "../../bolo/screentypes.h"
#include "../positions.h"
#include "SDL.h"
#include "boloicon.xpm"
#include "cursor.h"
#include "dialogabout.h"
#include "dialoggamefinder.h"
#include "dialoggameinfo.h"
#include "dialoggamesetup.h"
#include "dialogkeysetup.h"
#include "dialogmessages.h"
#include "dialognetworkinfo.h"
#include "dialogopening.h"
#include "dialogpassword.h"
#include "dialogsetname.h"
#include "dialogsysteminfo.h"
#include "dialogudpsetup.h"
#include "draw.h"
#include "e_map.h"
#include "framemutex.h"
#include "frontend.h"
#include "gamefront.h"
#include "input.h"
#include "messagebox.h"
#include "preferences.h"
#include "sound.h"

/* Fix for LGM click lockup under linux. I can't process the event from the
 * callback as gtk/sdl locks break the system so we queue it for now then
 * process it in the next screenGameTick() */
typedef struct {
  bool used;
  BYTE x;
  BYTE y;
  BYTE bs;
  bool changeUsed;
  BYTE old_val;
  BYTE new_val;
} linuxLgmFix;

static linuxLgmFix llf;
static std::shared_mutex llfMutex;

#define DIALOG_BOX_TITLE "LinBolo"

/* typedef int DWORD; */
/* The size of the main window EXCLUDING Menus and Toolbar */
#define SCREEN_SIZE_X 515
#define SCREEN_SIZE_Y 325
/* Frame Rates */
/* Numbers slightly off because of timer innacuracies */
#define FRAME_RATE_60 111
#define STR_PLAYER_LEN 2

#define timeGetTime() SDL_GetTicks()

/* Defines the number of milliseconds in a second */
/* Used for frame rate counting */
#define MILLISECONDS 1000

/* Zoom Factors */
#define ZOOM_FACTOR_NORMAL 1
#define ZOOM_FACTOR_DOUBLE 2
#define ZOOM_FACTOR_QUAD 4

bool isTutorial;
bool isInMenu = false;
static bool isQuiting = FALSE;

/* Time since the game timer last went up */
static DWORD oldTick;
static DWORD ttick;
static DWORD oldFrameTick;

/* Stuff for the system info dialog box */
static DWORD dwSysFrameTotal = 0;
DWORD dwSysFrame = 0;
static DWORD dwSysGameTotal = 0;
static DWORD dwSysGame = 0;
static DWORD dwSysBrainTotal = 0;
static DWORD dwSysBrain = 0;

bool doingTutorial = false;
/* Current building item selected */
static buildSelect BsLinuxCurrent = BsTrees;

/* Whether the Gunsight is shown or not */
static bool showGunsight = FALSE;

/* Whether the sound effects are turn on or not */
bool soundEffects = false;
/* Do we play background sound */
static bool backgroundSound = TRUE;

/* Is the sound card of the ISA variety */
static bool isISASoundCard = TRUE;
static bool doneInitTutorial = FALSE;

/*
 *  new players */
static bool allowNewPlayers = TRUE;
static bool allowAlianceRequest = TRUE;

/* 5 Message Shown Status */
static bool showNewswireMessages = TRUE;
static bool showAssistantMessages = TRUE;
static bool showAIMessages = FALSE;
static bool showNetworkStatusMessages = TRUE;
static bool showNetworkDebugMessages = FALSE;

static bool useAutoslow; /* Other key preferences options */
static bool useAutohide;

/* Winbolo.net settings */
static char gameFrontWbnPass[FILENAME_MAX];
static bool gameFrontWbnUse;
static bool gameFrontWbnSavePass;

/* Automatic Scrolling or not */
static bool autoScrollingEnabled = FALSE;

/* Whether Pillbox & base labels should be shown */
bool showPillLabels = false;
bool showBaseLabels = false;

/* Labels of stuff */
static bool labelSelf = FALSE;
static labelLen labelMsg = lblShort;
static labelLen labelTank = lblShort;

#define itoa(X) std::format("{}", (int)X)

static keyItems keys;

GtkWidget *window;
GtkWidget *menu_bar;
GtkWidget *drawingarea1;
static GtkWidget *windowSysInfo;
static GtkWidget *windowNetInfo;
static GtkWidget *windowGameInfo;
static GtkWidget *windowMessages;
static GtkWidget *change_player_name1;
GSList *brainsGroup = nullptr;
GtkWidget *brains1_menu;

SDL_TimerID timerGameID = nullptr;
SDL_TimerID timerFrameID = nullptr;

gchar *applicationPath;

void windowStartTutorial();
static void menus(GtkWidget *window);
bool brainsHandlerLoadBrains(GtkWidget *hWnd);
void brainsHandlerManual(GtkWidget *hWnd);
void clientMutexWaitFor(void);
void clientMutexRelease(void);
bool brainHandlerIsBrainRunning();
void brainHandlerRun(GtkWidget *hWnd);
void brainsHandlerSet(GtkWidget *hWnd, bool enabled);

void gameFrontCloseServer();
static const int frameRateTime = (int)(MILLISECONDS / FRAME_RATE_60) - 1;

extern int timerGameInfo;
extern int sysInfoTimer;
extern int timerNetworkInfo;

/* To be implemented by someone... */
int moveMousePointer(updateType value) { return 0; }

/** FIXME THIS ENTIRE SECTION NEEDS TO BE BROUGHT UP TO LATEST VERSION.... Added
 * to enable compilation */
bool screenLoadCompressedMap() { return FALSE; }

/** FIXME THIS ENTIRE SECTION NEEDS TO BE BROUGHT UP TO LATEST VERSION.... Added
 * to enable compilation */
int serverMainGetTicks() { return 0; }

/** FIXME THIS ENTIRE SECTION NEEDS TO BE BROUGHT UP TO LATEST VERSION.... Added
 * to enable compilation */
int windowsGetTicks() { return 0; }

gint windowclose(GtkWidget *widget, gpointer gdata) {
  isInMenu = TRUE;
  if (screenGetGameRunning() == TRUE) {
    useAutoslow = screenGetTankAutoSlowdown();
    useAutohide = screenGetTankAutoHideGunsight();
  } else {
    gtk_main_quit();
    exit(0);
  }
  brainsHandlerManual(nullptr);
  isInMenu = FALSE;
  gameFrontCloseServer();
  if (timerGameID != nullptr) {
    SDL_RemoveTimer(timerGameID);
    timerGameID = nullptr;
  }
  usleep(2500);
  if (timerFrameID != nullptr) {
    SDL_RemoveTimer(timerFrameID);
    timerFrameID = nullptr;
  }

  gameFrontPutPrefs(&keys);

  if (windowMessages != nullptr) {
    gtk_widget_destroy(windowMessages);
    windowMessages = nullptr;
  }
  if (windowGameInfo != nullptr) {
    gtk_timeout_remove(timerGameInfo);
    gtk_widget_destroy(windowGameInfo);
    windowGameInfo = nullptr;
  }
  if (windowSysInfo != nullptr) {
    gtk_timeout_remove(sysInfoTimer);
    gtk_widget_destroy(windowSysInfo);
    windowSysInfo = nullptr;
  }

  if (windowNetInfo != nullptr) {
    gtk_timeout_remove(timerNetworkInfo);
    gtk_widget_destroy(windowNetInfo);
    windowNetInfo = nullptr;
  }
  // gdk_key_repeat_restore();
  gtk_main_quit();
  return FALSE;
}

char saveFileName[FILENAME_MAX];
GtkWidget *saveFileW;

void saveFileCancel(GtkWidget *w, GtkFileSelection *fs) {
  gtk_widget_destroy(saveFileW);
}

void saveFileOK(GtkWidget *w, GtkFileSelection *fs) {
  const gchar *sTempFile =
      gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));
  strcpy(saveFileName, sTempFile);
  gtk_widget_destroy(saveFileW);
}

gint saveFileDestroy(GtkWidget *widget, gpointer *data) {
  gtk_grab_remove(widget);
  gtk_main_quit();
  return (FALSE);
}

GtkRequisition req;
gint configure_event(GtkWidget *widget, GdkEventConfigure *event) {
  GdkPixmap *pixmap;
  GdkBitmap *mask;
  pixmap = gdk_pixmap_create_from_xpm_d(window->window, &mask,
                                        &window->style->bg[GTK_STATE_NORMAL],
                                        (gchar **)boloicon_xpm);
  gdk_window_set_icon(window->window, nullptr, pixmap, mask);
  return TRUE;
}
GtkWidget *request_alliance1;
GtkWidget *leave_alliance1;
GtkWidget *show_gunsight1;
GtkWidget *automatic_scrolling1;
GtkWidget *manual1;
GtkWidget *idc_player1;
GtkWidget *idc_player2;
GtkWidget *idc_player3;
GtkWidget *idc_player4;
GtkWidget *idc_player5;
GtkWidget *idc_player6;
GtkWidget *idc_player7;
GtkWidget *idc_player8;
GtkWidget *idc_player9;
GtkWidget *idc_player10;
GtkWidget *idc_player11;
GtkWidget *idc_player12;
GtkWidget *idc_player13;
GtkWidget *idc_player14;
GtkWidget *idc_player15;
GtkWidget *idc_player16;
GtkWidget *background_sound1;
GtkWidget *sound_effects1;

gboolean press_event(GtkWidget *widget, GdkEventKey *event,
                     gpointer user_data) {
  long e = event->keyval;

  e = gdk_keyval_to_upper(e);
  inputButtonInput(&keys, e, TRUE);
  if (e == keys.kiTankView) {
    screenTankView();
  }
  if (e == keys.kiPillView) {
    screenPillView(0, 0);
  }
  return TRUE;
}

gboolean release_event(GtkWidget *widget, GdkEventKey *event,
                       gpointer user_data) {
  inputButtonInput(&keys, gdk_keyval_to_upper(event->keyval), FALSE);
  return TRUE;
}

gint windowLoseFocus(GtkWidget *widget, gpointer data) {
  // gdk_key_repeat_restore();
  return 0;
}

gint windowGetFocus(GtkWidget *widget, gpointer data) {
  if (doneInitTutorial == FALSE && isTutorial == TRUE) {
    doneInitTutorial = TRUE;
    windowStartTutorial();
  }
  frameMutexWaitFor();
  clientMutexWaitFor();
  std::vector<baseAlliance> bas;
  bas.push_back(baseNeutral);
  BYTE count = screenNumBases();
  for (int i = 1; i <= count; count++) {
    bas.push_back(screenBaseAlliance(i));
  }
  std::vector<pillAlliance> pas;
  count = screenNumPills();
  pas.push_back(pillNeutral);
  for (int i = 1; i <= count; count++) {
    pas.push_back(screenPillAlliance(i));
  }
  std::vector<bolo::tankAlliance> tas;
  count = screenNumPills();
  tas.push_back(bolo::tankAlliance::tankNone);
  for (int i = 1; i <= count; count++) {
    tas.push_back(screenTankAlliance(i));
  }
  drawRedrawAll(SCREEN_SIZE_X, SCREEN_SIZE_Y, BsLinuxCurrent, bas, pas, tas,
                std::nullopt, false, false);
  clientMutexRelease();
  frameMutexRelease();
  // gdk_key_repeat_disable();
  return 0;
}

/* The frame rate */
int frameRate = FRAME_RATE_60;

int drawGetFrameRate();

Uint32 windowGameTimer(Uint32 interval, void *param) {
  static bool inBrain = FALSE;  /* Are we allready calling the brain? */
  static bool justKeys = FALSE; /* Just the keys tick or whole game? */
  static BYTE t2 = 0;
  static int trackerTime = 11500; /* When we should update the tracker */
  tankButton tb;                  /* Buttons being pressed */
  bool isShoot; /* Whether the tank fire key is being pressed */
  bool used = FALSE;
  bool brainRunning = brainHandlerIsBrainRunning();
  ttick = timeGetTime();
  /* Update the game objects if required */
  if ((ttick - oldTick) > GAME_TICK_LENGTH) {
    /* Get the keyboard state */
    while ((ttick - oldTick) > GAME_TICK_LENGTH) {
      if (doingTutorial == FALSE) {
        if (justKeys == TRUE) {
          if (brainRunning == FALSE) {
            tb = inputGetKeys(FALSE);
          } else {
            inputScroll(FALSE);
          }
          clientMutexWaitFor();
          screenKeysTick(tb, brainRunning);
          clientMutexRelease();
          justKeys = FALSE;
        } else {
          t2++;
          trackerTime++;
          if (brainRunning == FALSE) {
            tb = inputGetKeys(FALSE);
            isShoot = inputIsFireKeyPressed(FALSE);
          } else {
            inputScroll(FALSE);
          }
          frameMutexWaitFor();
          clientMutexWaitFor();
          llfMutex.lock();
          if (llf.used == TRUE) {
            screenManMove((buildSelect)llf.bs);
            llf.used = FALSE;
          }
          if (llf.changeUsed == TRUE) {
            screenGetFrontend()->selectIndent((buildSelect)llf.old_val,
                                              (buildSelect)llf.new_val);
            llf.changeUsed = FALSE;
          }
          screenGameTick(tb, isShoot, brainRunning);
          llfMutex.unlock();
          clientMutexRelease();
          frameMutexRelease();
          justKeys = TRUE;
          used = TRUE;
        }
        oldTick += GAME_TICK_LENGTH;
        if (oldTick > ttick) {
          oldTick = ttick;
        }
      }
    }
  }
  dwSysGame += (timeGetTime() - ttick);
  if (used == TRUE) {
    netMakeDataPosPacket();
    netMakeTokenPacket();
  }
  /* AI */
  if (used == TRUE && inBrain == FALSE && brainRunning == TRUE &&
      netGetStatus() != netFailed) {
    clientMutexWaitFor();
    inBrain = TRUE;
    clientMutexRelease();
    ttick = timeGetTime();
    brainHandlerRun(nullptr);
    clientMutexWaitFor();
    inBrain = FALSE;
    clientMutexRelease();
    dwSysBrain += timeGetTime() - ttick;
  }

  if (t2 >= 50) {
    netSecond();
    dwSysFrameTotal = dwSysFrame;
    dwSysGameTotal = dwSysGame;
    dwSysBrainTotal = dwSysBrain;
    dwSysBrain = 0;
    dwSysFrame = 0;
    dwSysGame = 0;
    t2 = 0;
  }

  if (trackerTime >= (12000)) {
    netSendTrackerUpdate();
    trackerTime = 0;
  }
  return interval;
}
bool hideMainView = false;

Uint32 windowFrameRateTimer(Uint32 interval, void *param) {
  DWORD tick;

  if (hideMainView == FALSE) {
    tick = timeGetTime();
    if ((int)(tick - oldFrameTick) >= frameRateTime) {
      frameMutexWaitFor();
      clientMutexWaitFor();
      screenUpdateTiles();
      screenGetFrontend()->drawAll();
      clientMutexRelease();
      frameMutexRelease();
      oldFrameTick = timeGetTime();
    }
    dwSysFrame += (timeGetTime() - tick);
  }
  return frameRateTime;
}

gboolean windowMouseMove(GtkWidget *widget, GdkEventMotion *event,
                         gpointer user_data) {
  int x, y;
  GdkModifierType state;
  BYTE cursorX, cursorY;

  if (event->is_hint && frameRateTime > 0) {
    gdk_window_get_pointer(event->window, &x, &y, &state);
    y -= req.height;
    cursorMove(x, y);
    if (cursorPos(x, y, &cursorX, &cursorY) == TRUE) {
      screenSetCursorPos(cursorX, cursorY);
    }
  }
  return TRUE;
}

void windowDisableSound() { gtk_widget_set_sensitive(sound_effects1, FALSE); }

/*********************************************************
 *NAME:          windowSetKeys
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/1/99
 *LAST MODIFIED: 31/1/99
 *PURPOSE:
 * Sets the keys to be that held in value
 *
 *ARGUMENTS:
 *  value - Pointer to hold the copy of the keys
 *********************************************************/
void windowSetKeys(keyItems *value) {
  keys.kiForward = value->kiForward;
  keys.kiBackward = value->kiBackward;
  keys.kiLeft = value->kiLeft;
  keys.kiRight = value->kiRight;
  keys.kiShoot = value->kiShoot;
  keys.kiLayMine = value->kiLayMine;
  keys.kiGunIncrease = value->kiGunIncrease;
  keys.kiGunDecrease = value->kiGunDecrease;
  keys.kiTankView = value->kiTankView;
  keys.kiPillView = value->kiPillView;
  keys.kiScrollUp = value->kiScrollUp;
  keys.kiScrollDown = value->kiScrollDown;
  keys.kiScrollLeft = value->kiScrollLeft;
  keys.kiScrollRight = value->kiScrollRight;
}

char messageBody[16 * 1024];
char messageTitle[256];
char messageBody2[16 * 1024];
char messageTitle2[256];
char messageBody3[16 * 1024];
char messageTitle3[256];
uint8_t numMessages = 0;

gint windowMessageCheck(gpointer data) {
  if (numMessages == 1) {
    numMessages = 0;
    gdk_threads_enter();
    MessageBox(messageBody, messageTitle);
    gdk_threads_leave();
    oldTick = timeGetTime();
    ttick = oldTick;
    doingTutorial = FALSE;
  } else if (numMessages > 1) {
    BYTE temp = numMessages;
    numMessages = 0;
    /* Process each message until FALSE Max messages is 3*/
    gdk_threads_enter();
    switch (temp) {
      case 3:
        MessageBox(messageBody3, messageTitle3);
      case 2:
        MessageBox(messageBody2, messageTitle2);
      case 1:
        MessageBox(messageBody, messageTitle);
    }
    gdk_threads_leave();
    oldTick = timeGetTime();
    ttick = oldTick;
    doingTutorial = FALSE;
  }
  return 100;
}

gboolean windowMouseClick(GtkWidget *widget, GdkEventButton *event,
                          gpointer user_data) {
  int xPos;
  int yPos;
  std::optional<buildSelect> newSelect; /* The new selection */
  BYTE xValue; /* X & Y values if clicked inside main view*/
  BYTE yValue;

  xPos = event->x;
  yPos = event->y;
  yPos -= req.height;
  if (cursorPos(xPos, yPos, &xValue, &yValue) == TRUE) {
    std::unique_lock l(llfMutex);
    llf.bs = BsLinuxCurrent;
    llf.used = TRUE;
  } else {
    if (xPos >= BS_TREE_OFFSET_X &&
        xPos <= (BS_TREE_OFFSET_X + BS_ITEM_SIZE_X) &&
        yPos >= BS_TREE_OFFSET_Y &&
        yPos <= (BS_TREE_OFFSET_Y + BS_ITEM_SIZE_Y)) {
      newSelect = BsTrees;
    } else if (xPos >= BS_ROAD_OFFSET_X &&
               xPos <= (BS_ROAD_OFFSET_X + BS_ITEM_SIZE_X) &&
               yPos >= BS_ROAD_OFFSET_Y &&
               yPos <= (BS_ROAD_OFFSET_Y + BS_ITEM_SIZE_Y)) {
      newSelect = BsRoad;
    } else if (xPos >= BS_BUILDING_OFFSET_X &&
               xPos <= (BS_BUILDING_OFFSET_X + BS_ITEM_SIZE_X) &&
               yPos >= BS_BUILDING_OFFSET_Y &&
               yPos <= (BS_BUILDING_OFFSET_Y + BS_ITEM_SIZE_Y)) {
      newSelect = BsBuilding;
    } else if (xPos >= BS_PILLBOX_OFFSET_X &&
               xPos <= (BS_PILLBOX_OFFSET_X + BS_ITEM_SIZE_X) &&
               yPos >= BS_PILLBOX_OFFSET_Y &&
               yPos <= (BS_PILLBOX_OFFSET_Y + BS_ITEM_SIZE_Y)) {
      newSelect = BsPillbox;
    } else if (xPos >= BS_MINE_OFFSET_X &&
               xPos <= (BS_MINE_OFFSET_X + BS_ITEM_SIZE_X) &&
               yPos >= BS_MINE_OFFSET_Y &&
               yPos <= (BS_MINE_OFFSET_Y + BS_ITEM_SIZE_Y)) {
      newSelect = BsMine;
    }

    if (newSelect.has_value() && newSelect != BsLinuxCurrent) {
      std::unique_lock l(llfMutex);
      llf.changeUsed = TRUE;
      llf.old_val = BsLinuxCurrent;
      llf.new_val = newSelect.value();
      BsLinuxCurrent = newSelect.value();
    }
  }

  return TRUE;
}

GtkWidget *windowCreate() {
  GtkWidget *ret = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  //   gtk_window_set_default_size (GTK_WINDOW (ret), 400, 400);
  gtk_window_set_policy(GTK_WINDOW(ret), FALSE, FALSE, FALSE);
  gtk_widget_set_app_paintable(ret, FALSE);
  gtk_widget_add_events(ret, GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK |
                                 GDK_POINTER_MOTION_HINT_MASK |
                                 GDK_KEY_RELEASE_MASK);
  gtk_signal_connect(GTK_OBJECT(ret), "button_press_event",
                     GTK_SIGNAL_FUNC(windowMouseClick), nullptr);
  gtk_signal_connect(GTK_OBJECT(ret), "motion_notify_event",
                     GTK_SIGNAL_FUNC(windowMouseMove), nullptr);
  gtk_signal_connect(GTK_OBJECT(ret), "delete_event",
                     GTK_SIGNAL_FUNC(windowclose), nullptr);
  gtk_signal_connect(GTK_OBJECT(ret), "configure_event",
                     GTK_SIGNAL_FUNC(configure_event), nullptr);
  gtk_signal_connect(GTK_OBJECT(ret), "key_press_event",
                     GTK_SIGNAL_FUNC(press_event), nullptr);
  gtk_signal_connect(GTK_OBJECT(ret), "key_release_event",
                     GTK_SIGNAL_FUNC(release_event), nullptr);
  gtk_signal_connect(GTK_OBJECT(ret), "focus_out_event",
                     GTK_SIGNAL_FUNC(windowLoseFocus), nullptr);
  gtk_timeout_add(500, windowMessageCheck, nullptr);

  menus(ret);
  gtk_widget_size_request(menu_bar, &req);
  gtk_widget_set_usize(ret, SCREEN_SIZE_X, SCREEN_SIZE_Y + req.height);
  gtk_window_set_title(GTK_WINDOW(ret), "LinBolo");
  gtk_widget_realize(ret);
  gtk_widget_realize(drawingarea1);
  return ret;
}

/* Called on game start to set the backend options */
void sendBackendMenuOptions() {
  screenSetTankLabelLen(labelTank);
  screenSetGunsight(showGunsight);
  screenSetAutoScroll(autoScrollingEnabled);
  screenSetTankAutoSlowdown(useAutoslow);
  screenSetTankAutoHideGunsight(useAutohide);
  screenSetLabelOwnTank(labelSelf);
  screenSetMesageLabelLen(labelMsg);
  screenShowMessages(MSG_NEWSWIRE, showNewswireMessages);
  screenShowMessages(MSG_ASSISTANT, showAssistantMessages);
  screenShowMessages(MSG_AI, showAIMessages);
  screenShowMessages(MSG_NETSTATUS, showNetworkStatusMessages);
  screenShowMessages(MSG_NETWORK, showNetworkDebugMessages);

  isInMenu = TRUE;
  if (soundIsPlayable() == FALSE) {
    /* Disable windows */
    windowDisableSound();
  } else {
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sound_effects1),
                                   soundEffects);
  }
  isInMenu = FALSE;
}

//-------------------------------------GAMEFRONT---
//----
//

/* Game playing options */
char fileName[FILENAME_MAX]; /* filename and path of the map to use or command
                                line arguement */
char password[MAP_STR_SIZE]; /* game password */
bool hiddenMines;            /* Hidden mines allowed */
aiType compTanks = aiNone;   /* Whether computer tanks are allowed */
gameType gametype;           /* The type of game being played */
long startDelay;             /* The start delay */
long timeLen;                /* Game time length */
bool gameFrontRemeber;       /* Remeber player name? */

#define NUM_SECONDS 60

/* UDP stuff */
char gameFrontName[PLAYER_NAME_LEN];    /* Player Name */
char gameFrontUdpAddress[FILENAME_MAX]; /* IP of target machine */
unsigned short gameFrontMyUdp;
unsigned short gameFrontTargetUdp;

char gameFrontTrackerAddr[FILENAME_MAX];
unsigned short gameFrontTrackerPort;
bool gameFrontTrackerEnabled;

/* Dialog states */
openingStates dlgState = openStart;

bool isServer = FALSE; /* Are we server of a net game */

bool wantRejoin; /* Do we want to rejoin */
FILE *fpServer = nullptr;

/*********************************************************
 *NAME:          gameFrontGetWinbolonetSettings
 *AUTHOR:        John Morrison
 *CREATION DATE: 24/06/00
 *LAST MODIFIED: 24/06/00
 *PURPOSE:
 * Gets the winbolo.net settings for the setup dialog
 *
 *ARGUMENTS:
 * password - Destination Password
 * useWbn   - Destination  Should we participate in wbn
 * savePass - Destination for should save password
 *********************************************************/
void gameFrontGetWinbolonetSettings(char *password, bool *useWbn,
                                    bool *savePass) {
  *useWbn = gameFrontWbnUse;
  *savePass = gameFrontWbnSavePass;
  strcpy(password, gameFrontWbnPass);
}

/*********************************************************
 *NAME:          gameFrontSetWinbolonetSettings
 *AUTHOR:        John Morrison
 *CREATION DATE: 24/06/00
 *LAST MODIFIED: 24/06/00
 *PURPOSE:
 * Sets the winbolo.net settings from the setup dialog
 *
 *ARGUMENTS:
 * password - Password
 * useWbn   - Should we participate in winbolo.net
 * savePass - Should we save the password?
 *********************************************************/
void gameFrontSetWinbolonetSettings(char *password, bool useWbn,
                                    bool savePass) {
  strcpy(gameFrontWbnPass, password);
  gameFrontWbnUse = useWbn;
  gameFrontWbnSavePass = savePass;
}

/*********************************************************
 *NAME:          gameFrontSetupServer
 *AUTHOR:        John Morrison
 *CREATION DATE: 3/11/99
 *LAST MODIFIED:  3/1/00
 *PURPOSE:
 * Attempts to start the server process. Returns success
 *
 *ARGUMENTS:
 *
 *********************************************************/
bool gameFrontSetupServer() {
  bool returnValue;       /* Value to return                 */
  char cmdLine[1024];     /* Command Line                   */
  char tmp[FILENAME_MAX]; /* Temp String */
  long l;

  MessageBox(langGetText(STR_GAMEFRONT_SERVERSTARTMSG), DIALOG_BOX_TITLE);

  returnValue = TRUE;
  cmdLine[0] = EMPTY_CHAR;

  /* Make the command line */
  if (strcmp(fileName, "") != 0) {
    strcat(cmdLine, " -map ");
    strcat(cmdLine, "\"");
    strcat(cmdLine, fileName);
    strcat(cmdLine, "\"");
  } else {
    strcat(cmdLine, "-inbuilt");
  }
  strcat(cmdLine, " ");
  strcat(cmdLine, "-port ");
  sprintf(tmp, "%d ", gameFrontTargetUdp);
  strcat(cmdLine, tmp);
  strcat(cmdLine, " -gametype ");
  switch (gametype) {
    case gameOpen:
      strcat(cmdLine, "open ");
      break;
    case gameTournament:
      strcat(cmdLine, "tournament ");
      break;
    case gameStrictTournament:
      strcat(cmdLine, "strict ");
      break;
  }
  strcat(cmdLine, " -mines ");
  if (hiddenMines == TRUE) {
    strcat(cmdLine, "yes ");
  } else {
    strcat(cmdLine, "no ");
  }
  strcat(cmdLine, " -ai ");
  switch (compTanks) {
    case aiNone:
      strcat(cmdLine, "no ");
      break;
    case aiYes:
      strcat(cmdLine, "yes ");
      break;
    default:
      /* Yes with an advantage */
      strcat(cmdLine, "yesAdv ");
      break;
  }
  strcat(cmdLine, " -delay ");
  if (startDelay > 0) {
    l = startDelay;
    l /= GAME_NUMGAMETICKS_SEC;
  } else {
    l = 0;
  }
  sprintf(tmp, "%ld ", l);
  strcat(cmdLine, tmp);
  strcat(cmdLine, " -limit ");
  if (timeLen > 0) {
    l = timeLen;
    l /= NUM_SECONDS;
    l /= GAME_NUMGAMETICKS_SEC;
  } else {
    l = timeLen;
  }
  sprintf(tmp, "%ld ", l);
  strcat(cmdLine, tmp);
  if (gameFrontTrackerEnabled == TRUE && dlgState != openLanSetup) {
    /* Add on the tracker info */
    strcat(cmdLine, "-tracker ");
    sprintf(tmp, "%s:%d ", gameFrontTrackerAddr, gameFrontTrackerPort);
    strcat(cmdLine, tmp);
  }
  if (password[0] != '\0') {
    strcat(cmdLine, " -password ");
    strcat(cmdLine, password);
  }
  strcat(cmdLine, " -quiet ");
  if (gameFrontWbnUse == FALSE) {
    strcat(cmdLine, " -nowinbolonet");
  }
  strcpy(tmp, "./linbolods ");
  strcat(tmp, cmdLine);
  strcat(tmp, "  2> /dev/null");
  fpServer = popen(tmp, "w");
  if (!fpServer) {
    returnValue = FALSE;
  } else {
    sleep(5);
  }
  return returnValue;
}

void gameFrontCloseServer() {
  if (fpServer) {
    fputs("quit\n", fpServer);
    fflush(fpServer);
    pclose(fpServer);
  }
  fpServer = nullptr;
}

/*********************************************************
 *NAME:          gameFrontGetPlayerName
 *AUTHOR:        John Morrison
 *CREATION DATE: 24/2/99
 *LAST MODIFIED: 24/2/99
 *PURPOSE:
 * Gets the player name
 *
 *ARGUMENTS:
 *  pn       - Pointer to hold the player name
 *********************************************************/
void gameFrontGetPlayerName(char *pn) { strcpy(pn, gameFrontName); }

/*********************************************************
 *NAME:          gameFrontSetPlayerName
 *AUTHOR:        John Morrison
 *CREATION DATE: 24/2/99
 *LAST MODIFIED:  2/6/00
 *PURPOSE:
 * Sets the player name
 *
 *ARGUMENTS:
 *  pn       - Player name to set to
 *********************************************************/
void gameFrontSetPlayerName(char *pn) { strcpy(gameFrontName, pn); }

/*********************************************************
 *NAME:          gameFrontSetAIType
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/2/99
 *LAST MODIFIED: 13/11/99
 *PURPOSE:
 * Sets the AI type of the game. (From networking module)
 *
 *ARGUMENTS:
 *
 *********************************************************/
void gameFrontSetAIType(aiType ait) {
  compTanks = ait;
  screenSetAiType(compTanks);
  if (compTanks == aiNone) {
    brainsHandlerSet(nullptr, FALSE);
  } else {
    brainsHandlerSet(nullptr, TRUE);
  }
}

/*********************************************************
 *NAME:          gameFrontGetPassword
 *AUTHOR:        John Morrison
 *CREATION DATE: 24/2/99
 *LAST MODIFIED: 24/2/99
 *PURPOSE:
 * The network module has tried to join a game with a
 * password, request it here.
 *
 *ARGUMENTS:
 * pword - Password slected
 *********************************************************/
void gameFrontGetPassword(char *pword) {
  GtkWidget *p;
  password[0] = '\0';
  p = dialogPasswordCreate();
  gtk_widget_show(p);
  gtk_grab_add(p);
  gtk_main();
  strcpy(pword, password);
}

/*********************************************************
 *NAME:          gameFrontSetRemeber
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/4/99
 *LAST MODIFIED: 19/4/99
 *PURPOSE:
 * Sets whether we remeber the player name or not
 *
 *ARGUMENTS:
 *  isSet - Value to set it to
 *********************************************************/
void gameFrontSetRemeber(bool isSet) { gameFrontRemeber = isSet; }

/*********************************************************
 *NAME:          gameFrontGetRemeber
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/4/99
 *LAST MODIFIED: 19/4/99
 *PURPOSE:
 * Gets whether we remeber the player name or not
 *
 *ARGUMENTS:
 *
 *********************************************************/
bool gameFrontGetRemeber() { return gameFrontRemeber; }

/*********************************************************
 *NAME:          gameFrontSetFileName
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/1/99
 *LAST MODIFIED: 28/1/99
 *PURPOSE:
 * Sets the map to use. Maps have been verified as OK
 *
 *ARGUMENTS:
 *  getName - Holds the filename
 *********************************************************/
void gameFrontSetFileName(const char *getName) { strcpy(fileName, getName); }

/*********************************************************
 *NAME:          gameFrontGetUdpOptions
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/2/99
 *LAST MODIFIED: 21/2/99
 *PURPOSE:
 * Gets the UDP options
 *
 *ARGUMENTS:
 *  pn       - Pointer to hold the player name
 *  add      - Pointer to hold target machine address
 *  theirUdp - Pointer to hold target machine UDP port
 *  myUdp    - Pointer to this machine UDP port
 *********************************************************/
void gameFrontGetUdpOptions(char *pn, char *add, unsigned short *theirUdp,
                            unsigned short *myUdp) {
  strcpy(pn, gameFrontName);
  strcpy(add, gameFrontUdpAddress);
  *myUdp = gameFrontMyUdp;
  *theirUdp = gameFrontTargetUdp;
}

/*********************************************************
 *NAME:          gameFrontSetUdpOptions
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/2/99
 *LAST MODIFIED: 21/2/99
 *PURPOSE:
 * Gets the UDP options
 *
 *ARGUMENTS:
 *  pn       - Player name
 *  add      - Target machine address
 *  theirUdp - Target machine UDP port
 *  myUdp    - This machine UDP port
 *********************************************************/
void gameFrontSetUdpOptions(char *pn, char *add, unsigned short theirUdp,
                            unsigned short myUdp) {
  strcpy(gameFrontName, pn);
  strcpy(gameFrontUdpAddress, add);
  gameFrontMyUdp = myUdp;
  gameFrontTargetUdp = theirUdp;
}

/*********************************************************
 *NAME:          gameFrontGetTrackerOptions
 *AUTHOR:        John Morrison
 *CREATION DATE: 13/11/99
 *LAST MODIFIED: 13/11/99
 *PURPOSE:
 * Gets the tracker options
 *
 *ARGUMENTS:
 *  address - Buffer to hold address
 *  port    - Pointer to hold port
 *  enabled - Pointer to hold enabled flag
 *********************************************************/
void gameFrontGetTrackerOptions(char *address, unsigned short *port,
                                bool *enabled) {
  strcpy(address, gameFrontTrackerAddr);
  *port = gameFrontTrackerPort;
  *enabled = gameFrontTrackerEnabled;
}

void startTimers() {
  oldFrameTick = SDL_GetTicks();
  oldTick = SDL_GetTicks();
  timerGameID = SDL_AddTimer(
      GAME_TICK_LENGTH, (SDL_NewTimerCallback)windowGameTimer, (void *)nullptr);

  timerFrameID =
      SDL_AddTimer(frameRateTime, (SDL_NewTimerCallback)windowFrameRateTimer,
                   (void *)nullptr);
  isQuiting = TRUE;
}

/*********************************************************
 *NAME:          gameFrontSetTrackerOptions
 *AUTHOR:        John Morrison
 *CREATION DATE: 13/11/99
 *LAST MODIFIED: 13/11/99
 *PURPOSE:
 * Sets the tracker options
 *
 *ARGUMENTS:
 *  address - New tracker address
 *  port    - New tracker port
 *  enabled - New tracker enabled flag
 *********************************************************/
void gameFrontSetTrackerOptions(char *address, unsigned short port,
                                bool enabled) {
  strcpy(gameFrontTrackerAddr, address);
  gameFrontTrackerPort = port;
  gameFrontTrackerEnabled = enabled;
}

/*********************************************************
 *NAME:          gameFrontEnableRejoin
 *AUTHOR:        John Morrison
 *CREATION DATE: 22/6/00
 *LAST MODIFIED: 22/6/00
 *PURPOSE:
 * Sets it so we want rejoin
 *
 *ARGUMENTS:
 *
 *********************************************************/
void gameFrontEnableRejoin() { wantRejoin = TRUE; }

/*********************************************************
 *NAME:          gameFrontGetGameOptions
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/4/99
 *LAST MODIFIED: 19/4/99
 *PURPOSE:
 * Gets the game options.
 *
 *ARGUMENTS:
 *  pword  - Holds the password. (Empty if none)
 *  gtype  - Holds the game type.
 *  hm     - Are hidden mines allowed?
 *  ai     - Are computer tanks allowed etc.
 *  sd     - Game start delay
 *  tlimit - Game time limit
 *********************************************************/
void gameFrontGetGameOptions(char *pword, gameType *gt, bool *hm, aiType *ai,
                             long *sd, long *tlimit) {
  strcpy(pword, password);
  *gt = gametype;
  *hm = hiddenMines;
  *ai = compTanks;
  *sd = startDelay;
  *tlimit = timeLen;
}

/*********************************************************
 *NAME:          gameFrontSetGameOptions
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/1/99
 *LAST MODIFIED: 3/12/99
 *PURPOSE:
 * Sets the game options up.
 *
 *ARGUMENTS:
 *  pword    - Holds the password. (Empty if none)
 *  gtype    - Holds the game type.
 *  hm       - Are hidden mines allowed?
 *  ai       - Are computer tanks allowed etc.
 *  sd       - Game start delay
 *  tlimit   - Game time limit
 *  justPass - TRUE if we want to just set the password
 *********************************************************/
void gameFrontSetGameOptions(char *pword, gameType gt, bool hm, aiType ai,
                             long sd, long tlimit, bool justPass) {
  strcpy(password, pword);
  if (justPass == FALSE) {
    gametype = gt;
    hiddenMines = hm;
    compTanks = ai;
    if (compTanks == aiNone) {
      brainsHandlerSet(nullptr, FALSE);
    } else {
      brainsHandlerSet(nullptr, TRUE);
    }
    startDelay = sd;
    timeLen = tlimit;
  }
}

/*********************************************************
 *NAME:          gameFrontLoadTutorial
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/5/00
 *LAST MODIFIED: 1/5/00
 *PURPOSE:
 * Attempts to load the built in tutorial by loading the
 * compress resource. Returns Success
 *
 *ARGUMENTS:
 *
 *********************************************************/
bool gameFrontLoadTutorial() {
  bool returnValue; /* Value to return */
  // BYTE buff[6000] = TUT_MAP;

  returnValue = FALSE;  // screenLoadCompressedMap(buff, 4182, "Inbuilt
                        // Tutorial", gameStrictTournament, FALSE, 0, -1,
                        // langGetText(STR_DLGGAMESETUP_DEFAULTNAME), FALSE);
  if (returnValue == TRUE) {
    netSetup(netSingle, gameFrontMyUdp, gameFrontUdpAddress, gameFrontTargetUdp,
             password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort,
             gameFrontTrackerEnabled, wantRejoin, FALSE, "");
    startTimers();
  }

  return returnValue;
}

bool startSinglePlayer();

bool gameFrontSetDlgState(GtkWidget *oldWindow, openingStates newState) {
  GtkWidget *newWindow;
  auto frontend = std::make_unique<bolo::LinuxFrontend>();
  if (newState == openSetup || newState == openUdpSetup ||
      newState == openInternetSetup || newState == openLanSetup) {
    dlgState = newState;
    newWindow = dialogGameSetupCreate();
    gtk_widget_show(newWindow);
    gtk_widget_hide(oldWindow);
  } else if ((dlgState == openUdpSetup || dlgState == openInternetSetup ||
              dlgState == openLanSetup) &&
             newState == openFinished) {
    if (gameFrontSetupServer() == TRUE) {
      screenSetup((gameType)0, std::move(frontend), FALSE, 0,
                  UNLIMITED_GAME_TIME);
      if (netSetup(netUdp, gameFrontMyUdp, "127.0.0.1", gameFrontTargetUdp,
                   password, FALSE, gameFrontTrackerAddr, gameFrontTrackerPort,
                   gameFrontTrackerEnabled, wantRejoin, gameFrontWbnUse,
                   gameFrontWbnPass) == FALSE) {
        wantRejoin = FALSE;
        MessageBox(langGetText(STR_GAMEFRONTERR_JOINGAME), DIALOG_BOX_TITLE);
        netDestroy();
        screenDestroy();
        return FALSE;
      } else {
        dlgState = openFinished;
        sendBackendMenuOptions();
        startTimers();
        gtk_widget_destroy(oldWindow);
        gtk_widget_show(window);
      }
    } else {
      MessageBox(langGetText(STR_GAMEFRONTERR_SPAWNSERVER), DIALOG_BOX_TITLE);
      dlgState = openStart;
    }

  } else if (newState == openLan) {
    newWindow = dialogGameFinderCreate(
        FALSE, langGetText(STR_GAMEFRONT_LANFINDER_TITLE));
    gtk_widget_show(newWindow);
    gtk_widget_hide(oldWindow);
    dlgState = newState;
  } else if (newState == openInternet) {
    newWindow = dialogGameFinderCreate(
        TRUE, langGetText(STR_GAMEFRONT_TRACKERFINDER_TITLE));
    gtk_widget_show(newWindow);
    gtk_widget_hide(oldWindow);
    dlgState = newState;
  } else if (newState == openUdp || newState == openInternetManual ||
             newState == openLanManual) {
    newWindow = dialogUdpSetupCreate();
    gtk_widget_show(newWindow);
    gtk_widget_hide(oldWindow);
  } else if ((dlgState == openInternet || dlgState == openLan ||
              dlgState == openUdp || dlgState == openLanManual ||
              dlgState == openInternetManual) &&
             newState == openUdpJoin) {
    screenSetup((gameType)0, std::move(frontend), FALSE, 0,
                UNLIMITED_GAME_TIME);
    if (netSetup(netUdp, gameFrontMyUdp, gameFrontUdpAddress,
                 gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr,
                 gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin,
                 gameFrontWbnUse, gameFrontWbnPass) == FALSE) {
      wantRejoin = FALSE;
      MessageBox(langGetText(STR_GAMEFRONTERR_JOINGAME), DIALOG_BOX_TITLE);
      netDestroy();
      screenDestroy();
      gtk_widget_hide(oldWindow);
      gtk_widget_show(oldWindow);
      return FALSE;
    } else {
      oldFrameTick = SDL_GetTicks();
      oldTick = SDL_GetTicks();
      sendBackendMenuOptions();
      timerGameID =
          SDL_AddTimer(GAME_TICK_LENGTH, (SDL_NewTimerCallback)windowGameTimer,
                       (void *)nullptr);
      timerFrameID = SDL_AddTimer(frameRateTime,
                                  (SDL_NewTimerCallback)windowFrameRateTimer,
                                  (void *)nullptr);
      isQuiting = TRUE;
      gtk_widget_show(window);
      gtk_widget_destroy(oldWindow);
    }
  } else if ((dlgState == openInternetManual ||
              dlgState == openInternetSetup) &&
             newState == openWelcome) {
    dlgState = openInternet;
    newWindow = dialogGameFinderCreate(
        TRUE, langGetText(STR_GAMEFRONT_TRACKERFINDER_TITLE));
    gtk_widget_show(newWindow);
    gtk_widget_hide(oldWindow);
  } else if ((dlgState == openLanSetup || dlgState == openLanManual) &&
             newState == openWelcome) {
    dlgState = openLan;
    newWindow = dialogGameFinderCreate(
        FALSE, langGetText(STR_GAMEFRONT_LANFINDER_TITLE));
    gtk_widget_show(newWindow);
    gtk_widget_hide(oldWindow);
  } else if (newState == openTutorial) {
    doneInitTutorial = FALSE;
    gameFrontLoadTutorial();
    screenNetSetupTankGo();
    isTutorial = TRUE;
    gtk_widget_show(window);
    gtk_widget_hide(oldWindow);
  } else if (newState == openFinished) {
    startSinglePlayer();
    gtk_widget_show(window);
    gtk_widget_hide(oldWindow);
  } else if (newState == openWelcome) {
    if (oldWindow != nullptr) {
      gtk_widget_hide(oldWindow);
    }
    newWindow = dialogOpeningCreate();
    gtk_widget_show(newWindow);
  } else {
    dlgState = newState;
  }

  dlgState = newState;
  return TRUE;
}

void catch_alarm(int sig) { windowclose(window, nullptr); }

int main(int argc, char **argv) {
  char SDL_windowhack[32];
  GtkWidget *dialogOpening;
  gchar *mkDirPath;

  signal(SIGINT, catch_alarm);
  applicationPath = g_get_current_dir();

  /* Linux LGM fix */
  llf.used = FALSE;
  llf.changeUsed = FALSE;

  /* Make bolo preferences directory and brains directory */
  mkDirPath = g_strconcat(g_get_home_dir(), "/.linbolo", NULL);
  mkdir(mkDirPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  g_free(mkDirPath);
  mkDirPath = g_strconcat(g_get_home_dir(), "/.linbolo/brains", NULL);
  mkdir(mkDirPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  g_free(mkDirPath);

  gtk_init(&argc, &argv);

  timeLen = UNLIMITED_GAME_TIME;
  isTutorial = FALSE;
  dlgState = openStart;
  password[0] = '\0';
  gameFrontName[0] = '\0';
  gameFrontUdpAddress[0] = '\0';
  wantRejoin = FALSE;
  gameFrontTrackerEnabled = 1;
  gameFrontGetPrefs(&keys, &useAutoslow, &useAutohide);
  window = windowCreate();

  XSync(GDK_DISPLAY(), FALSE);
  /* Hack to get SDL to use GTK window */
  // sprintf(SDL_windowhack, "SDL_WINDOWID=%ld",
  // GDK_WINDOW_XWINDOW(window->window));
  sprintf(SDL_windowhack, "SDL_WINDOWID=%ld",
          GDK_WINDOW_XWINDOW(drawingarea1->window));
  putenv(SDL_windowhack);
  brainsHandlerLoadBrains(nullptr);

  /* Initialize SDL */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    gtk_main_quit();
    exit(0);
  }

  isQuiting = soundSetup();
  if (isQuiting == TRUE) {
    isQuiting = drawSetup();
  }
  if (isQuiting == TRUE) {
    isQuiting = inputSetup();
  }

  if (isQuiting == TRUE) {
    isQuiting = cursorSetup();
  }

  isQuiting ^= 1;

  while (isQuiting == FALSE) {
    gameFrontGetPrefs(&keys, &useAutoslow, &useAutohide);
    dialogOpening = dialogOpeningCreate();
    gtk_widget_show(dialogOpening);
    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
    // gdk_key_repeat_restore();
    if (isQuiting == FALSE) {
      gtk_widget_hide(window);
    }
    netDestroy();
    gameFrontPutPrefs(&keys);
    screenDestroy();
  }
  drawCleanup();
  soundCleanup();
  cursorCleanup();
  inputCleanup();
  SDL_Quit();
  g_free(applicationPath);
  return 0;
}

bool startSinglePlayer() {
  bool returnValue = TRUE;
  if (netSetup(netSingle, gameFrontMyUdp, gameFrontUdpAddress,
               gameFrontTargetUdp, password, FALSE, gameFrontTrackerAddr,
               gameFrontTrackerPort, gameFrontTrackerEnabled, wantRejoin, FALSE,
               "") == FALSE) {
    wantRejoin = FALSE;
    MessageBox(langGetText(STR_GAMEFRONTERR_NETSINGLEPLAYER), DIALOG_BOX_TITLE);
    netDestroy();
    screenDestroy();
    dlgState = openSetup;
    returnValue = FALSE;
  } else {
    auto frontend = std::make_unique<bolo::LinuxFrontend>();
    if (strcmp(fileName, "") != 0) {
      std::ifstream input(fileName);
      screenLoadMap(input, bolo::utilExtractMapName(fileName).c_str(), gametype,
                    std::move(frontend), hiddenMines, startDelay, timeLen,
                    gameFrontName, false);
    } else {
      std::istringstream input(std::string((const char *)e_map, e_map_len));
      screenLoadMap(input, "Everard Island", gametype, std::move(frontend),
                    hiddenMines, startDelay, timeLen, gameFrontName, false);
    }
  }
  if (returnValue == TRUE) {
    sendBackendMenuOptions();
    startTimers();
  }
  return FALSE;
}

/*********************************************************
 *NAME:          windowGetDrawTime
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/1/99
 *LAST MODIFIED: 30/1/99
 *PURPOSE:
 *  Returns number of miliseconds spent drawing
 *  last second
 *
 *ARGUMENTS:
 *
 *********************************************************/
int windowGetDrawTime(void) { return dwSysFrameTotal; }

/*********************************************************
 *NAME:          windowGetNetTime
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/1/99
 *LAST MODIFIED: 23/1/99
 *PURPOSE:
 *  Returns number of miliseconds spent doing network work
 *  last second
 *
 *ARGUMENTS:
 *
 *********************************************************/
int windowGetNetTime(void) { return netGetNetTime(); }

/*********************************************************
 *NAME:          windowGetAiTime
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/99
 *LAST MODIFIED: 26/11/99
 *PURPOSE:
 *  Returns number of miliseconds spent doing ai work
 *  last second
 *
 *ARGUMENTS:
 *
 *********************************************************/
int windowGetAiTime(void) { return dwSysBrainTotal; }

/*********************************************************
 *NAME:          windowGetSimTime
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/1/99
 *LAST MODIFIED: 30/1/99
 *PURPOSE:
 *  Returns number of miliseconds spent doing sim modelling
 *  last second
 *
 *ARGUMENTS:
 *
 *********************************************************/
int windowGetSimTime(void) { return dwSysGameTotal; }

/*********************************************************
 *NAME:          gameFrontGetPrefs
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/4/99
 *LAST MODIFIED: 13/6/00
 *PURPOSE:
 * Gets the preferences from the preferences file. Returns
 * success.
 *
 *ARGUMENTS:
 *  keys - Pointer to keys structure
 *  useAutoslow - Pointer to hold auto slowdown
 *  useAutohide - Pointer to hold auto gunsight show/hide
 *********************************************************/
bool gameFrontGetPrefs(keyItems *keys, bool *useAutoslow, bool *useAutohide) {
  std::string buff; /* Read Buffer               */

  bolo::Preferences prefs(bolo::GetPreferenceFile());

  /* Player Name */
  buff = prefs.get("SETTINGS", "Player Name")
             .value_or(langGetText(STR_DLGGAMESETUP_DEFAULTNAME));
  strncpy(gameFrontName, buff.c_str(), PLAYER_NAME_LEN);

  /* Target Address */
  buff = prefs.get("SETTINGS", "Target Address").value_or("");
  strncpy(gameFrontUdpAddress, buff.c_str(), FILENAME_MAX);

  /* Target UDP Port */
  buff =
      prefs.get("SETTINGS", "Target UDP Port").value_or(itoa(DEFAULT_UDP_PORT));
  gameFrontTargetUdp = std::stoi(buff);

  /* My UDP Port */
  buff = prefs.get("SETTINGS", "UDP Port").value_or(itoa(DEFAULT_UDP_PORT));
  gameFrontMyUdp = std::stoi(buff);

  /* Keys */
  buff = prefs.get("KEYS", "Forward").value_or(itoa(DEFAULT_FORWARD));
  keys->kiForward = std::stoi(buff);
  buff = prefs.get("KEYS", "Backwards").value_or(itoa(DEFAULT_BACKWARD));
  keys->kiBackward = std::stoi(buff);
  buff = prefs.get("KEYS", "Left").value_or(itoa(DEFAULT_LEFT));
  keys->kiLeft = std::stoi(buff);
  buff = prefs.get("KEYS", "Right").value_or(itoa(DEFAULT_RIGHT));
  keys->kiRight = std::stoi(buff);
  buff = prefs.get("KEYS", "Shoot").value_or(itoa(DEFAULT_SHOOT));
  keys->kiShoot = std::stoi(buff);
  buff = prefs.get("KEYS", "Lay Mine").value_or(itoa(DEFAULT_LAY_MINE));
  keys->kiLayMine = std::stoi(buff);
  buff = prefs.get("KEYS", "Increase Range")
             .value_or(itoa(DEFAULT_SCROLL_GUNINCREASE));
  keys->kiGunIncrease = std::stoi(buff);
  buff = prefs.get("KEYS", "Decrease Range")
             .value_or(itoa(DEFAULT_SCROLL_GUNDECREASE));
  keys->kiGunDecrease = std::stoi(buff);
  buff = prefs.get("KEYS", "Tank View").value_or(itoa(DEFAULT_TANKVIEW));
  keys->kiTankView = std::stoi(buff);
  buff = prefs.get("KEYS", "Pill View").value_or(itoa(DEFAULT_PILLVIEW));
  keys->kiPillView = std::stoi(buff);
  buff = prefs.get("KEYS", "Scroll Up").value_or(itoa(DEFAULT_SCROLLUP));
  keys->kiScrollUp = std::stoi(buff);
  buff = prefs.get("KEYS", "Scroll Down").value_or(itoa(DEFAULT_SCROLLDOWN));
  keys->kiScrollDown = std::stoi(buff);
  buff = prefs.get("KEYS", "Scroll Left").value_or(itoa(DEFAULT_SCROLLLEFT));
  keys->kiScrollLeft = std::stoi(buff);
  buff = prefs.get("KEYS", "Scroll Right").value_or(itoa(DEFAULT_SCROLLRIGHT));
  keys->kiScrollRight = std::stoi(buff);

  /* Remeber */
  buff = prefs.get("SETTINGS", "Remember Player Name").value_or("Yes");
  gameFrontRemeber = YESNO_TO_TRUEFALSE(buff[0]);

  /* Game Options */
  buff = prefs.get("GAME OPTIONS", "Hidden Mines").value_or("No");
  hiddenMines = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("GAME OPTIONS", "Allow Computer Tanks").value_or("0");
  compTanks = (aiType)std::stoi(buff);
  buff = prefs.get("GAME OPTIONS", "Game Type").value_or("1");
  gametype = (gameType)std::stoi(buff);
  buff = prefs.get("GAME OPTIONS", "Start Delay").value_or("0");
  startDelay = std::stoi(buff);
  buff = prefs.get("GAME OPTIONS", "Time Length")
             .value_or(itoa(UNLIMITED_GAME_TIME));
  timeLen = atol(buff.c_str());
  buff = prefs.get("GAME OPTIONS", "Auto Slowdown").value_or("No");
  *useAutoslow = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("GAME OPTIONS", "Auto Show-Hide Gunsight").value_or("No");
  *useAutohide = YESNO_TO_TRUEFALSE(buff[0]);

  /* Tracker options */
  buff = prefs.get("TRACKER", "Address").value_or(TRACKER_ADDRESS);
  strncpy(gameFrontTrackerAddr, buff.c_str(), FILENAME_MAX);
  buff = prefs.get("TRACKER", "Port").value_or(itoa(TRACKER_PORT));
  gameFrontTrackerPort = std::stoi(buff);
  buff = prefs.get("TRACKER", "Enabled").value_or("No");
  gameFrontTrackerEnabled = YESNO_TO_TRUEFALSE(buff[0]);

  /* Menu Items */
  buff = prefs.get("MENU", "Show Gunsight").value_or("No");
  showGunsight = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Sound Effects").value_or("Yes");
  soundEffects = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Allow Background Sound").value_or("Yes");
  backgroundSound = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "ISA Sound Card").value_or("No");
  isISASoundCard = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show Gunsight").value_or("No");
  showGunsight = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show Newswire Messages").value_or("Yes");
  showNewswireMessages = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show Assistant Messages").value_or("Yes");
  showAssistantMessages = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show AI Messages").value_or("Yes");
  showAIMessages = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show Network Status Messages").value_or("Yes");
  showNetworkStatusMessages = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show Network Debug Messages").value_or("No");
  showNetworkDebugMessages = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Autoscroll Enabled").value_or("No");
  autoScrollingEnabled = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show Pill Labels").value_or("No");
  showPillLabels = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Show Base Labels").value_or("No");
  showBaseLabels = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Label Own Tank").value_or("No");
  labelSelf = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("MENU", "Window Size").value_or("1");
  // zoomFactor = std::stoi(buff);
  buff = prefs.get("MENU", "Message Label Size").value_or("1");
  labelMsg = (labelLen)std::stoi(buff);
  buff = prefs.get("MENU", "Tank Label Size").value_or("1");
  labelTank = (labelLen)std::stoi(buff);

  /* Winbolo.net */
  buff = prefs.get("WINBOLO.NET", "Password").value_or("");
  strncpy(gameFrontWbnPass, buff.c_str(), FILENAME_MAX);
  buff = prefs.get("WINBOLO.NET", "Active").value_or("No");
  gameFrontWbnUse = YESNO_TO_TRUEFALSE(buff[0]);
  buff = prefs.get("WINBOLO.NET", "Save Password").value_or("Yes");
  gameFrontWbnSavePass = YESNO_TO_TRUEFALSE(buff[0]);

  /* Load in the language */
  buff = prefs.get("SETTINGS", "Language").value_or("");
  // TODO: Set the language.

  return true;
}

/*********************************************************
 *NAME:          gameFrontPutPrefs
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/4/99
 *LAST MODIFIED: 13/6/00
 *PURPOSE:
 * Puts the preferences to the preferences file.
 *
 *ARGUMENTS:
 *  keys       - Pointer to keys structure
 *********************************************************/
void gameFrontPutPrefs(keyItems *keys) {
  char playerName[PLAYER_NAME_LEN];  /* Current player Name       */
  char playerName2[PLAYER_NAME_LEN]; /* Current player Name       */

  bolo::Preferences prefs(bolo::GetPreferenceFile());
  /* Player Name */
  if ((netGetType() == netSingle || gameFrontRemeber == TRUE) &&
      dlgState != openSetup) {
    screenGetPlayerName(playerName);
    if (playerName[0] == '*') {
      strcpy(playerName2, playerName + 1);
    } else {
      strcpy(playerName2, playerName);
    }
    prefs.set("SETTINGS", "Player Name", playerName2);
  } else {
    prefs.set("SETTINGS", "Player Name", gameFrontName);
  }

  /* Target Address */
  prefs.set("SETTINGS", "Target Address", gameFrontUdpAddress);

  /* Target UDP Port */
  prefs.set("SETTINGS", "Target UDP Port", itoa(gameFrontTargetUdp));
  /* My UDP Port */
  prefs.set("SETTINGS", "UDP Port", itoa(gameFrontMyUdp));
  /* Languages */
  char buff[512];
  langGetFileName(buff);
  prefs.set("SETTINGS", "Language", buff);

  /* Keys */
  prefs.set("KEYS", "Forward", itoa(keys->kiForward));
  prefs.set("KEYS", "Backwards", itoa(keys->kiBackward));
  prefs.set("KEYS", "Left", itoa(keys->kiLeft));
  prefs.set("KEYS", "Right", itoa(keys->kiRight));
  prefs.set("KEYS", "Shoot", itoa(keys->kiShoot));
  prefs.set("KEYS", "Lay Mine", itoa(keys->kiLayMine));
  prefs.set("KEYS", "Increase Range", itoa(keys->kiGunIncrease));
  prefs.set("KEYS", "Decrease Range", itoa(keys->kiGunDecrease));
  prefs.set("KEYS", "Tank View", itoa(keys->kiTankView));
  prefs.set("KEYS", "Pill View", itoa(keys->kiPillView));
  prefs.set("KEYS", "Scroll Up", itoa(keys->kiScrollUp));
  prefs.set("KEYS", "Scroll Down", itoa(keys->kiScrollDown));
  prefs.set("KEYS", "Scroll Left", itoa(keys->kiScrollLeft));
  prefs.set("KEYS", "Scroll Right", itoa(keys->kiScrollRight));
  /* Remember */
  prefs.set("SETTINGS", "Remember Player Name",
            TRUEFALSE_TO_STR(gameFrontRemeber));

  /* Options */
  prefs.set("GAME OPTIONS", "Hidden Mines", TRUEFALSE_TO_STR(hiddenMines));
  prefs.set("GAME OPTIONS", "Allow Computer Tanks", itoa((int)compTanks));
  prefs.set("GAME OPTIONS", "Game Type", itoa((int)gametype));
  prefs.set("GAME OPTIONS", "Start Delay", itoa(startDelay));
  prefs.set("GAME OPTIONS", "Time Length", itoa(timeLen));
  prefs.set("GAME OPTIONS", "Auto Slowdown", TRUEFALSE_TO_STR(useAutoslow));
  prefs.set("GAME OPTIONS", "Auto Show-Hide Gunsight",
            TRUEFALSE_TO_STR(useAutohide));

  /* Tracker */
  prefs.set("TRACKER", "Address", gameFrontTrackerAddr);
  prefs.set("TRACKER", "Port", itoa(gameFrontTrackerPort));
  prefs.set("TRACKER", "Enabled", TRUEFALSE_TO_STR(gameFrontTrackerEnabled));

  /* Menu Items */
  prefs.set("MENU", "Show Gunsight", TRUEFALSE_TO_STR(showGunsight));
  prefs.set("MENU", "Sound Effects", TRUEFALSE_TO_STR(soundEffects));

  prefs.set("MENU", "Allow Background Sound",
            TRUEFALSE_TO_STR(backgroundSound));
  prefs.set("MENU", "ISA Sound Card", TRUEFALSE_TO_STR(isISASoundCard));
  prefs.set("MENU", "Show Gunsight", TRUEFALSE_TO_STR(showGunsight));
  prefs.set("MENU", "Show Newswire Messages",
            TRUEFALSE_TO_STR(showNewswireMessages));
  prefs.set("MENU", "Show Assistant Messages",
            TRUEFALSE_TO_STR(showAssistantMessages));
  prefs.set("MENU", "Show AI Messages", TRUEFALSE_TO_STR(showAIMessages));
  prefs.set("MENU", "Show Network Status Messages",
            TRUEFALSE_TO_STR(showNetworkStatusMessages));
  prefs.set("MENU", "Show Network Debug Messages",
            TRUEFALSE_TO_STR(showNetworkDebugMessages));
  prefs.set("MENU", "Autoscroll Enabled",
            TRUEFALSE_TO_STR(autoScrollingEnabled));
  prefs.set("MENU", "Show Pill Labels", TRUEFALSE_TO_STR(showPillLabels));
  prefs.set("MENU", "Show Base Labels", TRUEFALSE_TO_STR(showBaseLabels));
  prefs.set("MENU", "Label Own Tank", TRUEFALSE_TO_STR(labelSelf));
  prefs.set("MENU", "Window Size", "1");
  prefs.set("MENU", "Message Label Size", itoa((int)labelMsg));
  prefs.set("MENU", "Tank Label Size", itoa((int)labelTank));

  /* Winbolo.net */
  if (gameFrontWbnSavePass == TRUE) {
    prefs.set("WINBOLO.NET", "Password", gameFrontWbnPass);
  } else {
    prefs.set("WINBOLO.NET", "Password", "");
  }
  prefs.set("WINBOLO.NET", "Active", TRUEFALSE_TO_STR(gameFrontWbnUse));
  prefs.set("WINBOLO.NET", "Save Password",
            TRUEFALSE_TO_STR(gameFrontWbnSavePass));
  std::string buff2 =
      prefs.get("WINBOLO.NET", "Host").value_or("wbn.winbolo.net");
  prefs.set("WINBOLO.NET", "Host", buff2);
}

BYTE upTo = 0;

void windowStartTutorial() {
  upTo = 0;
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(automatic_scrolling1),
                                 TRUE);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(show_gunsight1), TRUE);
  screenSetGunsight(TRUE);
  screenSetAutoScroll(TRUE);
  MessageBox(langGetText(STR_TUTORIAL_START01), DIALOG_BOX_TITLE);
  MessageBox(langGetText(STR_TUTORIAL_START02), DIALOG_BOX_TITLE);
  MessageBox(langGetText(STR_TUTORIAL_START03), DIALOG_BOX_TITLE);
  MessageBox(langGetText(STR_TUTORIAL_START04), DIALOG_BOX_TITLE);
  doingTutorial = FALSE;
  oldTick = timeGetTime();
  ttick = oldTick;
}

void on_new1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  isQuiting = FALSE;
  windowclose(window, nullptr);
}

void on_save_map1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  bool saveOK; /* Did the file save OK */

  /* Get the name of the map */
  screenGetMapName(saveFileName);
  strcat(saveFileName, ".map");
  saveFileW = gtk_file_selection_new("Save Map File...");
  gtk_signal_connect(GTK_OBJECT(saveFileW), "destroy",
                     (GtkSignalFunc)saveFileDestroy, &saveFileW);

  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(saveFileW)->ok_button),
                     "clicked", (GtkSignalFunc)saveFileOK, saveFileW);
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(saveFileW)->cancel_button),
                     "clicked", (GtkSignalFunc)saveFileCancel, saveFileW);
  gtk_file_selection_set_filename(GTK_FILE_SELECTION(saveFileW), saveFileName);
  gtk_widget_show(saveFileW);

  gtk_grab_add(saveFileW);
  saveFileName[0] = '\0';
  gtk_main();
  if (saveFileName[0] != '\0') {
    saveOK = screenSaveMap(saveFileName);
    if (saveOK == FALSE) {
      MessageBox(langGetText(STR_WBERR_SAVEMAP), DIALOG_BOX_TITLE);
    }
  }
}

void windowGameInfoClose() { windowGameInfo = nullptr; }

void on_game_information1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (windowGameInfo == nullptr) {
    windowGameInfo = dialogGameInfoCreate();
    gtk_widget_realize(windowGameInfo);
    gtk_widget_show(windowGameInfo);
  } else {
    gdk_window_raise((windowGameInfo->window));
  }
}

void windowSystemInfoClose() { windowSysInfo = nullptr; }

void on_system_information1_activate(GtkMenuItem *menuitem,
                                     gpointer user_data) {
  if (windowSysInfo == nullptr) {
    windowSysInfo = dialogSystemInfoCreate();
    gtk_widget_realize(windowSysInfo);
    gtk_widget_show(windowSysInfo);
  } else {
    gdk_window_raise((windowSysInfo->window));
  }
}

void windowNetworkInfoClose() { windowNetInfo = nullptr; }

void on_network_information1_activate(GtkMenuItem *menuitem,
                                      gpointer user_data) {
  if (windowNetInfo == nullptr) {
    windowNetInfo = dialogNetworkInformationCreate();
    gtk_window_set_policy(GTK_WINDOW(windowNetInfo), FALSE, FALSE, FALSE);
    gtk_widget_realize(windowNetInfo);
    gtk_widget_show(windowNetInfo);
  } else {
    gdk_window_raise((windowNetInfo->window));
  }
}

void on_exit1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  isQuiting = TRUE;
  windowclose(window, nullptr);
}

void on_window_size1_activate(GtkMenuItem *menuitem, gpointer user_data) {}

void on_automatic_scrolling1_activate(GtkMenuItem *menuitem,
                                      gpointer user_data) {
  autoScrollingEnabled ^= 1;
  screenSetAutoScroll(autoScrollingEnabled);
}

void on_show_gunsight1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    showGunsight ^= 1;
    screenSetGunsight(showGunsight);
  }
}

void on_message_sender_names1_activate(GtkMenuItem *menuitem,
                                       gpointer user_data) {}

void on_short1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  labelMsg = lblShort;
  screenSetMesageLabelLen(lblShort);
}

void on_long1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  labelMsg = lblLong;
  screenSetMesageLabelLen(lblLong);
}

void on_none1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  labelTank = lblNone;
  screenSetTankLabelLen(lblNone);
}

void on_short2_activate(GtkMenuItem *menuitem, gpointer user_data) {
  labelTank = lblShort;
  screenSetTankLabelLen(lblShort);
}

void on_long2_activate(GtkMenuItem *menuitem, gpointer user_data) {
  labelTank = lblLong;
  screenSetTankLabelLen(lblLong);
}

void on_don_t_label_own_tank1_activate(GtkMenuItem *menuitem,
                                       gpointer user_data) {
  labelSelf ^= 1;
  screenSetLabelOwnTank(labelSelf);
}

void on_pillbox_labels1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  showPillLabels ^= 1;
}

void on_refuelling_base_labels1_activate(GtkMenuItem *menuitem,
                                         gpointer user_data) {
  showBaseLabels ^= 1;
}

void on_hide_main_view1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  hideMainView ^= 1;
  if (hideMainView == TRUE) {
    frameMutexWaitFor();
    drawDownloadScreen(TRUE);
    frameMutexRelease();
  }
}

void on_allow_new_players1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  allowNewPlayers ^= 1;
  netSetAllowNewPlayers(allowNewPlayers);
}

void on_allowiance_request1_activate(GtkMenuItem *menuitem,
                                     gpointer user_data) {
  allowAlianceRequest ^= 1;
}

void on_set_keys1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  GtkWidget *k;

  k = dialogKeySetupCreate(&keys);
  gtk_widget_show(k);
  gtk_grab_add(k);
  gtk_main();
}

void on_change_player_name1_activate(GtkMenuItem *menuitem,
                                     gpointer user_data) {
  GtkWidget *g;

  g = dialogSetNameCreate(TRUE);
  gtk_widget_show(g);
  gtk_grab_add(g);
  gtk_main();
}

void on_sound_effects1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    isInMenu = TRUE;
    soundEffects ^= 1;
    //    gtk_widget_set_sensitive(background_sound1, soundEffects);
    isInMenu = FALSE;
  }
}

void on_background_sound1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  backgroundSound ^= 1;
}

void on_newswire_messages1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  showNewswireMessages ^= 1;
  screenShowMessages(MSG_NEWSWIRE, showNewswireMessages);
}

void on_assistant_messages1_activate(GtkMenuItem *menuitem,
                                     gpointer user_data) {
  showAssistantMessages ^= 1;
  screenShowMessages(MSG_ASSISTANT, showAssistantMessages);
}

void on_ai_brain_messages1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  showAIMessages ^= 1;
  screenShowMessages(MSG_AI, showAIMessages);
}

void on_network_status_messages1_activate(GtkMenuItem *menuitem,
                                          gpointer user_data) {
  showNetworkStatusMessages ^= 1;
  screenShowMessages(MSG_NETSTATUS, showNetworkStatusMessages);
}

void on_network_debug_messages1_activate(GtkMenuItem *menuitem,
                                         gpointer user_data) {
  showNetworkDebugMessages ^= 1;
  screenShowMessages(MSG_NETWORK, showNetworkDebugMessages);
}

void on_request_alliance1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  screenRequestAlliance();
}

void on_leave_alliance1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  screenLeaveAlliance();
}

void windowMessagesClose() { windowMessages = nullptr; }

void on_send_message1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (windowMessages == nullptr) {
    windowMessages = dialogMessagesCreate();
    gtk_widget_realize(windowMessages);
    gtk_widget_show(windowMessages);
  } else {
    gdk_window_raise((windowMessages->window));
  }
}

void on_select_all1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  screenCheckAllNonePlayers(TRUE);
}

void on_select_none1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  screenCheckAllNonePlayers(FALSE);
}

void on_select_allies1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  screenCheckAlliedPlayers();
}

void on_select_nearby_tanks1_activate(GtkMenuItem *menuitem,
                                      gpointer user_data) {
  screenCheckNearbyPlayers();
}

void on_player1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player01);
  }
}

void on_player_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player02);
  }
}

void on_player3_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player03);
  }
}

void on_player4_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player04);
  }
}

void on_player5_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player05);
  }
}

void on_player6_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player06);
  }
}

void on_player7_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player07);
  }
}

void on_player8_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player08);
  }
}

void on_player9_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player09);
  }
}

void on_player10_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player10);
  }
}

void on_player11_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player11);
  }
}

void on_player12_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player12);
  }
}

void on_player13_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player13);
  }
}

void on_player14_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player14);
  }
}

void on_player15_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player15);
  }
}

void on_player16_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    screenTogglePlayerCheckState(player16);
  }
}

void on_manual1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  if (isInMenu == FALSE) {
    isInMenu = TRUE;
    brainsHandlerManual(nullptr);
    isInMenu = FALSE;
  }
}

void on_help2_activate(GtkMenuItem *menuitem, gpointer user_data) {
  MessageBox("Please consult the manual.pdf file in the LinBolo directory.",
             DIALOG_BOX_TITLE);
}

void on_about1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  GtkWidget *about;

  about = dialogAboutCreate();
  gtk_widget_show(about);
  gtk_grab_add(about);
  gtk_main();
}

void on_normal1_activate(GtkMenuItem *menuitem, gpointer user_data) {}

void on_double1_activate(GtkMenuItem *menuitem, gpointer user_data) {}

void on_quad1_activate(GtkMenuItem *menuitem, gpointer user_data) {}

void menus(GtkWidget *window) {
  GtkWidget *vbox1;
  GtkWidget *file1;
  GtkWidget *file1_menu;
  GtkWidget *new1;
  GtkWidget *save_map1;
  GtkWidget *separator1;
  GtkWidget *game_information1;
  GtkWidget *system_information1;
  GtkWidget *network_information1;
  GtkWidget *background_sound1;
  GtkWidget *separator2;
  GtkWidget *exit1;
  GtkWidget *edit1;
  GtkWidget *edit1_menu;
  GtkWidget *window_size1;
  GtkWidget *window_size1_menu;
  GSList *_3_group = nullptr;
  GtkWidget *normal1;
  GtkWidget *double1;
  GtkWidget *quad1;
  GtkWidget *message_sender_names1;
  GtkWidget *message_sender_names1_menu;
  GSList *_0_group = nullptr;
  GtkWidget *short1;
  GtkWidget *long1;
  GtkWidget *tank_labels1;
  GtkWidget *tank_labels1_menu;
  GSList *_1_group = nullptr;
  GtkWidget *none1;
  GtkWidget *short2;
  GtkWidget *long2;
  GtkWidget *don_t_label_own_tank1;
  GtkWidget *pillbox_labels1;
  GtkWidget *refuelling_base_labels1;
  GtkWidget *separator3;
  GtkWidget *hide_main_view1;
  GtkWidget *linbolo1;
  GtkWidget *linbolo1_menu;
  GtkWidget *allow_new_players1;
  GtkWidget *allow_alliance_request1;
  GtkWidget *set_keys1;
  GtkWidget *separator4;
  GtkWidget *separator5;
  GtkWidget *newswire_messages1;
  GtkWidget *assistant_messages1;
  GtkWidget *ai_brain_messages1;
  GtkWidget *network_status_messages1;
  GtkWidget *network_debug_messages1;
  GtkWidget *separator6;
  GtkWidget *players1;
  GtkWidget *players1_menu;
  GtkWidget *send_message1;
  GtkWidget *separator7;
  GtkWidget *select_all1;
  GtkWidget *select_none1;
  GtkWidget *select_allies1;
  GtkWidget *select_nearby_tanks1;
  GtkWidget *separator8;
  GtkWidget *brains1;
  GtkWidget *separator9;
  GtkWidget *help1;
  GtkWidget *help1_menu;
  GtkWidget *help2;
  GtkWidget *about1;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new();

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_ref(vbox1);
  gtk_object_set_data_full(GTK_OBJECT(window), "vbox1", vbox1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(vbox1);
  gtk_container_add(GTK_CONTAINER(window), vbox1);

  menu_bar = gtk_menu_bar_new();
  gtk_widget_ref(menu_bar);
  gtk_object_set_data_full(GTK_OBJECT(window), "menu_bar", menu_bar,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(menu_bar);
  gtk_box_pack_start(GTK_BOX(vbox1), menu_bar, FALSE, FALSE, 0);

  file1 = gtk_menu_item_new_with_label("File");
  gtk_widget_ref(file1);
  gtk_object_set_data_full(GTK_OBJECT(window), "file1", file1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(file1);
  gtk_container_add(GTK_CONTAINER(menu_bar), file1);

  file1_menu = gtk_menu_new();
  gtk_widget_ref(file1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "file1_menu", file1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file1), file1_menu);

  new1 = gtk_menu_item_new_with_label("New");
  gtk_widget_ref(new1);
  gtk_object_set_data_full(GTK_OBJECT(window), "new1", new1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(new1);
  gtk_container_add(GTK_CONTAINER(file1_menu), new1);

  save_map1 = gtk_menu_item_new_with_label("Save Map");
  gtk_widget_ref(save_map1);
  gtk_object_set_data_full(GTK_OBJECT(window), "save_map1", save_map1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(save_map1);
  gtk_container_add(GTK_CONTAINER(file1_menu), save_map1);
  gtk_widget_add_accelerator(save_map1, "activate", accel_group, GDK_S,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  separator1 = gtk_menu_item_new();
  gtk_widget_ref(separator1);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator1", separator1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator1);
  gtk_container_add(GTK_CONTAINER(file1_menu), separator1);
  gtk_widget_set_sensitive(separator1, FALSE);

  game_information1 = gtk_menu_item_new_with_label("Game Information");
  gtk_widget_ref(game_information1);
  gtk_object_set_data_full(GTK_OBJECT(window), "game_information1",
                           game_information1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(game_information1);
  gtk_container_add(GTK_CONTAINER(file1_menu), game_information1);

  system_information1 = gtk_menu_item_new_with_label("System Information");
  gtk_widget_ref(system_information1);
  gtk_object_set_data_full(GTK_OBJECT(window), "system_information1",
                           system_information1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(system_information1);
  gtk_container_add(GTK_CONTAINER(file1_menu), system_information1);

  network_information1 = gtk_menu_item_new_with_label("Network Information");
  gtk_widget_ref(network_information1);
  gtk_object_set_data_full(GTK_OBJECT(window), "network_information1",
                           network_information1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(network_information1);
  gtk_container_add(GTK_CONTAINER(file1_menu), network_information1);

  separator2 = gtk_menu_item_new();
  gtk_widget_ref(separator2);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator2", separator2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator2);
  gtk_container_add(GTK_CONTAINER(file1_menu), separator2);
  gtk_widget_set_sensitive(separator2, FALSE);

  exit1 = gtk_menu_item_new_with_label("Exit");
  gtk_widget_ref(exit1);
  gtk_object_set_data_full(GTK_OBJECT(window), "exit1", exit1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(exit1);
  gtk_container_add(GTK_CONTAINER(file1_menu), exit1);
  gtk_widget_add_accelerator(exit1, "activate", accel_group, GDK_Q,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  edit1 = gtk_menu_item_new_with_label("Edit");
  gtk_widget_ref(edit1);
  gtk_object_set_data_full(GTK_OBJECT(window), "edit1", edit1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(edit1);
  gtk_container_add(GTK_CONTAINER(menu_bar), edit1);

  edit1_menu = gtk_menu_new();
  gtk_widget_ref(edit1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "edit1_menu", edit1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit1), edit1_menu);

  window_size1 = gtk_menu_item_new_with_label("Window Size");
  gtk_widget_ref(window_size1);
  gtk_object_set_data_full(GTK_OBJECT(window), "window_size1", window_size1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(window_size1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), window_size1);

  window_size1_menu = gtk_menu_new();
  gtk_widget_ref(window_size1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "window_size1_menu",
                           window_size1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(window_size1), window_size1_menu);

  normal1 = gtk_radio_menu_item_new_with_label(_3_group, "Normal");
  _3_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(normal1));
  gtk_widget_ref(normal1);
  gtk_object_set_data_full(GTK_OBJECT(window), "normal1", normal1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(normal1);
  gtk_container_add(GTK_CONTAINER(window_size1_menu), normal1);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(normal1), TRUE);

  double1 = gtk_radio_menu_item_new_with_label(_3_group, "Double");
  _3_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(double1));
  gtk_widget_ref(double1);
  gtk_object_set_data_full(GTK_OBJECT(window), "double1", double1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(double1);
  gtk_container_add(GTK_CONTAINER(window_size1_menu), double1);

  quad1 = gtk_radio_menu_item_new_with_label(_3_group, "Quad");
  _3_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(quad1));
  gtk_widget_ref(quad1);
  gtk_object_set_data_full(GTK_OBJECT(window), "quad1", quad1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(quad1);
  gtk_container_add(GTK_CONTAINER(window_size1_menu), quad1);

  automatic_scrolling1 =
      gtk_check_menu_item_new_with_label("Automatic Scrolling");
  gtk_widget_ref(automatic_scrolling1);
  gtk_object_set_data_full(GTK_OBJECT(window), "automatic_scrolling1",
                           automatic_scrolling1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(automatic_scrolling1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), automatic_scrolling1);
  gtk_widget_add_accelerator(automatic_scrolling1, "activate", accel_group,
                             GDK_A, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  show_gunsight1 = gtk_check_menu_item_new_with_label("Show Gunsight");
  gtk_widget_ref(show_gunsight1);
  gtk_object_set_data_full(GTK_OBJECT(window), "show_gunsight1", show_gunsight1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(show_gunsight1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), show_gunsight1);
  gtk_widget_add_accelerator(show_gunsight1, "activate", accel_group, GDK_G,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  message_sender_names1 = gtk_menu_item_new_with_label("Message Sender Names");
  gtk_widget_ref(message_sender_names1);
  gtk_object_set_data_full(GTK_OBJECT(window), "message_sender_names1",
                           message_sender_names1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(message_sender_names1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), message_sender_names1);

  message_sender_names1_menu = gtk_menu_new();
  gtk_widget_ref(message_sender_names1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "message_sender_names1_menu",
                           message_sender_names1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(message_sender_names1),
                            message_sender_names1_menu);

  short1 = gtk_radio_menu_item_new_with_label(_0_group, "Short");
  _0_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(short1));
  gtk_widget_ref(short1);
  gtk_object_set_data_full(GTK_OBJECT(window), "short1", short1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(short1);
  gtk_container_add(GTK_CONTAINER(message_sender_names1_menu), short1);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(short1), TRUE);

  long1 = gtk_radio_menu_item_new_with_label(_0_group, "Long");
  _0_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(long1));
  gtk_widget_ref(long1);
  gtk_object_set_data_full(GTK_OBJECT(window), "long1", long1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(long1);
  gtk_container_add(GTK_CONTAINER(message_sender_names1_menu), long1);

  tank_labels1 = gtk_menu_item_new_with_label("Tank Labels");
  gtk_widget_ref(tank_labels1);
  gtk_object_set_data_full(GTK_OBJECT(window), "tank_labels1", tank_labels1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(tank_labels1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), tank_labels1);

  tank_labels1_menu = gtk_menu_new();
  gtk_widget_ref(tank_labels1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "tank_labels1_menu",
                           tank_labels1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(tank_labels1), tank_labels1_menu);

  none1 = gtk_radio_menu_item_new_with_label(_1_group, "None");
  _1_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(none1));
  gtk_widget_ref(none1);
  gtk_object_set_data_full(GTK_OBJECT(window), "none1", none1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(none1);
  gtk_container_add(GTK_CONTAINER(tank_labels1_menu), none1);
  gtk_widget_add_accelerator(none1, "activate", accel_group, GDK_1,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  short2 = gtk_radio_menu_item_new_with_label(_1_group, "Short");
  _1_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(short2));
  gtk_widget_ref(short2);
  gtk_object_set_data_full(GTK_OBJECT(window), "short2", short2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(short2);
  gtk_container_add(GTK_CONTAINER(tank_labels1_menu), short2);
  gtk_widget_add_accelerator(short2, "activate", accel_group, GDK_2,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(short2), TRUE);

  long2 = gtk_radio_menu_item_new_with_label(_1_group, "Long");
  _1_group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(long2));
  gtk_widget_ref(long2);
  gtk_object_set_data_full(GTK_OBJECT(window), "long2", long2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(long2);
  gtk_container_add(GTK_CONTAINER(tank_labels1_menu), long2);
  gtk_widget_add_accelerator(long2, "activate", accel_group, GDK_3,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  don_t_label_own_tank1 =
      gtk_check_menu_item_new_with_label("Don't label own tank");
  gtk_widget_ref(don_t_label_own_tank1);
  gtk_object_set_data_full(GTK_OBJECT(window), "don_t_label_own_tank1",
                           don_t_label_own_tank1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(don_t_label_own_tank1);
  gtk_container_add(GTK_CONTAINER(tank_labels1_menu), don_t_label_own_tank1);

  pillbox_labels1 = gtk_check_menu_item_new_with_label("Pillbox Labels");
  gtk_widget_ref(pillbox_labels1);
  gtk_object_set_data_full(GTK_OBJECT(window), "pillbox_labels1",
                           pillbox_labels1, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(pillbox_labels1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), pillbox_labels1);
  gtk_widget_add_accelerator(pillbox_labels1, "activate", accel_group, GDK_P,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  refuelling_base_labels1 =
      gtk_check_menu_item_new_with_label("Refuelling Base Labels");
  gtk_widget_ref(refuelling_base_labels1);
  gtk_object_set_data_full(GTK_OBJECT(window), "refuelling_base_labels1",
                           refuelling_base_labels1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(refuelling_base_labels1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), refuelling_base_labels1);
  gtk_widget_add_accelerator(refuelling_base_labels1, "activate", accel_group,
                             GDK_B, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  separator3 = gtk_menu_item_new();
  gtk_widget_ref(separator3);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator3", separator3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator3);
  gtk_container_add(GTK_CONTAINER(edit1_menu), separator3);
  gtk_widget_set_sensitive(separator3, FALSE);

  hide_main_view1 = gtk_check_menu_item_new_with_label("Hide Main View");
  gtk_widget_ref(hide_main_view1);
  gtk_object_set_data_full(GTK_OBJECT(window), "hide_main_view1",
                           hide_main_view1, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hide_main_view1);
  gtk_container_add(GTK_CONTAINER(edit1_menu), hide_main_view1);
  gtk_widget_add_accelerator(hide_main_view1, "activate", accel_group, GDK_H,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  linbolo1 = gtk_menu_item_new_with_label("LinBolo");
  gtk_widget_ref(linbolo1);
  gtk_object_set_data_full(GTK_OBJECT(window), "linbolo1", linbolo1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(linbolo1);
  gtk_container_add(GTK_CONTAINER(menu_bar), linbolo1);

  linbolo1_menu = gtk_menu_new();
  gtk_widget_ref(linbolo1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "linbolo1_menu", linbolo1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(linbolo1), linbolo1_menu);

  allow_new_players1 = gtk_check_menu_item_new_with_label("Allow New Players");
  gtk_widget_ref(allow_new_players1);
  gtk_object_set_data_full(GTK_OBJECT(window), "allow_new_players1",
                           allow_new_players1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(allow_new_players1), TRUE);
  gtk_widget_show(allow_new_players1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), allow_new_players1);

  allow_alliance_request1 =
      gtk_check_menu_item_new_with_label("Allow Alliance Request");
  gtk_widget_ref(allow_alliance_request1);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(allow_alliance_request1),
                                 TRUE);
  gtk_object_set_data_full(GTK_OBJECT(window), "allow_alliance_request",
                           allow_alliance_request1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(allow_alliance_request1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), allow_alliance_request1);

  set_keys1 = gtk_menu_item_new_with_label("Set Keys");
  gtk_widget_ref(set_keys1);
  gtk_object_set_data_full(GTK_OBJECT(window), "set_keys1", set_keys1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(set_keys1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), set_keys1);
  gtk_widget_add_accelerator(set_keys1, "activate", accel_group, GDK_K,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  change_player_name1 = gtk_menu_item_new_with_label("Change Player Name");
  gtk_widget_ref(change_player_name1);
  gtk_object_set_data_full(GTK_OBJECT(window), "change_player_name1",
                           change_player_name1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(change_player_name1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), change_player_name1);

  separator4 = gtk_menu_item_new();
  gtk_widget_ref(separator4);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator4", separator4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator4);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), separator4);
  gtk_widget_set_sensitive(separator4, FALSE);

  sound_effects1 = gtk_check_menu_item_new_with_label("Sound Effects");
  gtk_widget_ref(sound_effects1);
  gtk_object_set_data_full(GTK_OBJECT(window), "sound_effects1", sound_effects1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(sound_effects1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), sound_effects1);

  background_sound1 = gtk_check_menu_item_new_with_label("Background Sound");
  gtk_widget_ref(background_sound1);
  gtk_object_set_data_full(GTK_OBJECT(window), "background_sound1",
                           background_sound1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(background_sound1);
  gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(background_sound1), FALSE);
  gtk_widget_set_sensitive(background_sound1, FALSE);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), background_sound1);

  separator5 = gtk_menu_item_new();
  gtk_widget_ref(separator5);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator5", separator5,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator5);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), separator5);
  gtk_widget_set_sensitive(separator5, FALSE);

  newswire_messages1 = gtk_check_menu_item_new_with_label("Newswire Messages");
  gtk_widget_ref(newswire_messages1);
  gtk_object_set_data_full(GTK_OBJECT(window), "newswire_messages1",
                           newswire_messages1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(newswire_messages1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), newswire_messages1);

  assistant_messages1 =
      gtk_check_menu_item_new_with_label("Assistant Messages");
  gtk_widget_ref(assistant_messages1);
  gtk_object_set_data_full(GTK_OBJECT(window), "assistant_messages1",
                           assistant_messages1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(assistant_messages1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), assistant_messages1);

  ai_brain_messages1 = gtk_check_menu_item_new_with_label("AI Brain Messages");
  gtk_widget_ref(ai_brain_messages1);
  gtk_object_set_data_full(GTK_OBJECT(window), "ai_brain_messages1",
                           ai_brain_messages1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(ai_brain_messages1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), ai_brain_messages1);

  network_status_messages1 =
      gtk_check_menu_item_new_with_label("Network Status Messages");
  gtk_widget_ref(network_status_messages1);
  gtk_object_set_data_full(GTK_OBJECT(window), "network_status_messages1",
                           network_status_messages1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(network_status_messages1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), network_status_messages1);

  network_debug_messages1 =
      gtk_check_menu_item_new_with_label("Network Debug Messages");
  gtk_widget_ref(network_debug_messages1);
  gtk_object_set_data_full(GTK_OBJECT(window), "network_debug_messages1",
                           network_debug_messages1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(network_debug_messages1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), network_debug_messages1);

  separator6 = gtk_menu_item_new();
  gtk_widget_ref(separator6);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator6", separator6,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator6);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), separator6);
  gtk_widget_set_sensitive(separator6, FALSE);

  request_alliance1 = gtk_menu_item_new_with_label("Request Alliance");
  gtk_widget_ref(request_alliance1);
  gtk_object_set_data_full(GTK_OBJECT(window), "request_alliance1",
                           request_alliance1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(request_alliance1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), request_alliance1);
  gtk_widget_add_accelerator(request_alliance1, "activate", accel_group, GDK_R,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  leave_alliance1 = gtk_menu_item_new_with_label("Leave Alliance");
  gtk_widget_ref(leave_alliance1);
  gtk_object_set_data_full(GTK_OBJECT(window), "leave_alliance1",
                           leave_alliance1, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(leave_alliance1);
  gtk_container_add(GTK_CONTAINER(linbolo1_menu), leave_alliance1);

  players1 = gtk_menu_item_new_with_label("Players");
  gtk_widget_ref(players1);
  gtk_object_set_data_full(GTK_OBJECT(window), "players1", players1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(players1);
  gtk_container_add(GTK_CONTAINER(menu_bar), players1);

  players1_menu = gtk_menu_new();
  gtk_widget_ref(players1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "players1_menu", players1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(players1), players1_menu);

  send_message1 = gtk_menu_item_new_with_label("Send Message");
  gtk_widget_ref(send_message1);
  gtk_object_set_data_full(GTK_OBJECT(window), "send_message1", send_message1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(send_message1);
  gtk_container_add(GTK_CONTAINER(players1_menu), send_message1);
  gtk_widget_add_accelerator(send_message1, "activate", accel_group, GDK_M,
                             GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  separator7 = gtk_menu_item_new();
  gtk_widget_ref(separator7);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator7", separator7,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator7);
  gtk_container_add(GTK_CONTAINER(players1_menu), separator7);
  gtk_widget_set_sensitive(separator7, FALSE);

  select_all1 = gtk_menu_item_new_with_label("Select All");
  gtk_widget_ref(select_all1);
  gtk_object_set_data_full(GTK_OBJECT(window), "select_all1", select_all1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(select_all1);
  gtk_container_add(GTK_CONTAINER(players1_menu), select_all1);

  select_none1 = gtk_menu_item_new_with_label("Select None");
  gtk_widget_ref(select_none1);
  gtk_object_set_data_full(GTK_OBJECT(window), "select_none1", select_none1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(select_none1);
  gtk_container_add(GTK_CONTAINER(players1_menu), select_none1);

  select_allies1 = gtk_menu_item_new_with_label("Select Allies");
  gtk_widget_ref(select_allies1);
  gtk_object_set_data_full(GTK_OBJECT(window), "select_allies1", select_allies1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(select_allies1);
  gtk_container_add(GTK_CONTAINER(players1_menu), select_allies1);

  select_nearby_tanks1 = gtk_menu_item_new_with_label("Select Nearby Tanks");
  gtk_widget_ref(select_nearby_tanks1);
  gtk_object_set_data_full(GTK_OBJECT(window), "select_nearby_tanks1",
                           select_nearby_tanks1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(select_nearby_tanks1);
  gtk_container_add(GTK_CONTAINER(players1_menu), select_nearby_tanks1);

  separator8 = gtk_menu_item_new();
  gtk_widget_ref(separator8);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator8", separator8,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator8);
  gtk_container_add(GTK_CONTAINER(players1_menu), separator8);
  gtk_widget_set_sensitive(separator8, FALSE);

  idc_player1 = gtk_check_menu_item_new_with_label("1");
  gtk_widget_ref(idc_player1);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player1", idc_player1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player1);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player1);

  idc_player2 = gtk_check_menu_item_new_with_label("2");
  gtk_widget_ref(idc_player2);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player2", idc_player2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player2);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player2);

  idc_player3 = gtk_check_menu_item_new_with_label("3");
  gtk_widget_ref(idc_player3);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player3", idc_player3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player3);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player3);

  idc_player4 = gtk_check_menu_item_new_with_label("4");
  gtk_widget_ref(idc_player4);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player4", idc_player4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player4);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player4);

  idc_player5 = gtk_check_menu_item_new_with_label("5");
  gtk_widget_ref(idc_player5);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player5", idc_player5,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player5);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player5);

  idc_player6 = gtk_check_menu_item_new_with_label("6");
  gtk_widget_ref(idc_player6);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player6", idc_player6,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player6);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player6);

  idc_player7 = gtk_check_menu_item_new_with_label("7");
  gtk_widget_ref(idc_player7);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player7", idc_player7,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player7);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player7);

  idc_player8 = gtk_check_menu_item_new_with_label("8");
  gtk_widget_ref(idc_player8);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player8", idc_player8,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player8);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player8);

  idc_player9 = gtk_check_menu_item_new_with_label("9");
  gtk_widget_ref(idc_player9);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player9", idc_player9,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player9);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player9);

  idc_player10 = gtk_check_menu_item_new_with_label("10");
  gtk_widget_ref(idc_player10);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player10", idc_player10,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player10);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player10);

  idc_player11 = gtk_check_menu_item_new_with_label("11");
  gtk_widget_ref(idc_player11);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player11", idc_player11,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player11);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player11);

  idc_player12 = gtk_check_menu_item_new_with_label("12");
  gtk_widget_ref(idc_player12);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player12", idc_player12,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player12);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player12);

  idc_player13 = gtk_check_menu_item_new_with_label("13");
  gtk_widget_ref(idc_player13);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player13", idc_player13,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player13);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player13);

  idc_player14 = gtk_check_menu_item_new_with_label("14");
  gtk_widget_ref(idc_player14);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player14", idc_player14,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player14);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player14);

  idc_player15 = gtk_check_menu_item_new_with_label("15");
  gtk_widget_ref(idc_player15);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player15", idc_player15,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player15);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player15);

  idc_player16 = gtk_check_menu_item_new_with_label("16");
  gtk_widget_ref(idc_player16);
  gtk_object_set_data_full(GTK_OBJECT(window), "idc_player16", idc_player16,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_player16);
  gtk_container_add(GTK_CONTAINER(players1_menu), idc_player16);

  brains1 = gtk_menu_item_new_with_label("Brains");
  gtk_widget_ref(brains1);
  gtk_object_set_data_full(GTK_OBJECT(window), "brains1", brains1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(brains1);
  gtk_container_add(GTK_CONTAINER(menu_bar), brains1);

  brains1_menu = gtk_menu_new();
  gtk_widget_ref(brains1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "brains1_menu", brains1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(brains1), brains1_menu);

  manual1 = gtk_radio_menu_item_new_with_label(brainsGroup, "Manual");
  brainsGroup = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(manual1));

  gtk_widget_ref(manual1);
  gtk_object_set_data_full(GTK_OBJECT(window), "manual1", manual1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(manual1);
  gtk_container_add(GTK_CONTAINER(brains1_menu), manual1);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(manual1), TRUE);

  separator9 = gtk_menu_item_new();
  gtk_widget_ref(separator9);
  gtk_object_set_data_full(GTK_OBJECT(window), "separator9", separator9,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(separator9);
  gtk_container_add(GTK_CONTAINER(brains1_menu), separator9);
  gtk_widget_set_sensitive(separator9, FALSE);

  help1 = gtk_menu_item_new_with_label("Help");
  gtk_widget_ref(help1);
  gtk_object_set_data_full(GTK_OBJECT(window), "help1", help1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(help1);
  gtk_container_add(GTK_CONTAINER(menu_bar), help1);

  help1_menu = gtk_menu_new();
  gtk_widget_ref(help1_menu);
  gtk_object_set_data_full(GTK_OBJECT(window), "help1_menu", help1_menu,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(help1), help1_menu);

  help2 = gtk_menu_item_new_with_label("Help");
  gtk_widget_ref(help2);
  gtk_object_set_data_full(GTK_OBJECT(window), "help2", help2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(help2);
  gtk_container_add(GTK_CONTAINER(help1_menu), help2);

  about1 = gtk_menu_item_new_with_label("About");
  gtk_widget_ref(about1);
  gtk_object_set_data_full(GTK_OBJECT(window), "about1", about1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(about1);
  gtk_container_add(GTK_CONTAINER(help1_menu), about1);

  /* Apply check marks */
  switch (labelTank) {
    case lblNone:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(none1), TRUE);
      break;
    case lblShort:
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(short2), TRUE);
      break;
    default:
      /* case lblLong: */
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(long2), TRUE);
      break;
  }

  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(show_gunsight1),
                                 showGunsight);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(automatic_scrolling1),
                                 autoScrollingEnabled);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(don_t_label_own_tank1),
                                 !labelSelf);

  if (labelMsg == lblShort) {
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(short1), TRUE);
  } else {
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(long1), TRUE);
  }

  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(pillbox_labels1),
                                 showPillLabels);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(refuelling_base_labels1),
                                 showBaseLabels);

  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(newswire_messages1),
                                 showNewswireMessages);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(assistant_messages1),
                                 showAssistantMessages);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(ai_brain_messages1),
                                 showAIMessages);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(network_status_messages1),
                                 showNetworkStatusMessages);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(network_debug_messages1),
                                 showNetworkDebugMessages);
  gtk_signal_connect(GTK_OBJECT(new1), "activate",
                     GTK_SIGNAL_FUNC(on_new1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(save_map1), "activate",
                     GTK_SIGNAL_FUNC(on_save_map1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(game_information1), "activate",
                     GTK_SIGNAL_FUNC(on_game_information1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(system_information1), "activate",
                     GTK_SIGNAL_FUNC(on_system_information1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(network_information1), "activate",
                     GTK_SIGNAL_FUNC(on_network_information1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(exit1), "activate",
                     GTK_SIGNAL_FUNC(on_exit1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(window_size1), "activate",
                     GTK_SIGNAL_FUNC(on_window_size1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(normal1), "activate",
                     GTK_SIGNAL_FUNC(on_normal1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(double1), "activate",
                     GTK_SIGNAL_FUNC(on_double1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(quad1), "activate",
                     GTK_SIGNAL_FUNC(on_quad1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(automatic_scrolling1), "activate",
                     GTK_SIGNAL_FUNC(on_automatic_scrolling1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(show_gunsight1), "activate",
                     GTK_SIGNAL_FUNC(on_show_gunsight1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(message_sender_names1), "activate",
                     GTK_SIGNAL_FUNC(on_message_sender_names1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(short1), "activate",
                     GTK_SIGNAL_FUNC(on_short1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(long1), "activate",
                     GTK_SIGNAL_FUNC(on_long1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(none1), "activate",
                     GTK_SIGNAL_FUNC(on_none1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(short2), "activate",
                     GTK_SIGNAL_FUNC(on_short2_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(long2), "activate",
                     GTK_SIGNAL_FUNC(on_long2_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(don_t_label_own_tank1), "activate",
                     GTK_SIGNAL_FUNC(on_don_t_label_own_tank1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(pillbox_labels1), "activate",
                     GTK_SIGNAL_FUNC(on_pillbox_labels1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(refuelling_base_labels1), "activate",
                     GTK_SIGNAL_FUNC(on_refuelling_base_labels1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(hide_main_view1), "activate",
                     GTK_SIGNAL_FUNC(on_hide_main_view1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(allow_new_players1), "activate",
                     GTK_SIGNAL_FUNC(on_allow_new_players1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(allow_alliance_request1), "activate",
                     GTK_SIGNAL_FUNC(on_allowiance_request1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(set_keys1), "activate",
                     GTK_SIGNAL_FUNC(on_set_keys1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(change_player_name1), "activate",
                     GTK_SIGNAL_FUNC(on_change_player_name1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(sound_effects1), "activate",
                     GTK_SIGNAL_FUNC(on_sound_effects1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(background_sound1), "activate",
                     GTK_SIGNAL_FUNC(on_background_sound1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(newswire_messages1), "activate",
                     GTK_SIGNAL_FUNC(on_newswire_messages1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(assistant_messages1), "activate",
                     GTK_SIGNAL_FUNC(on_assistant_messages1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(ai_brain_messages1), "activate",
                     GTK_SIGNAL_FUNC(on_ai_brain_messages1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(network_status_messages1), "activate",
                     GTK_SIGNAL_FUNC(on_network_status_messages1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(network_debug_messages1), "activate",
                     GTK_SIGNAL_FUNC(on_network_debug_messages1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(request_alliance1), "activate",
                     GTK_SIGNAL_FUNC(on_request_alliance1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(leave_alliance1), "activate",
                     GTK_SIGNAL_FUNC(on_leave_alliance1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(send_message1), "activate",
                     GTK_SIGNAL_FUNC(on_send_message1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(select_all1), "activate",
                     GTK_SIGNAL_FUNC(on_select_all1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(select_none1), "activate",
                     GTK_SIGNAL_FUNC(on_select_none1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(select_allies1), "activate",
                     GTK_SIGNAL_FUNC(on_select_allies1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(select_nearby_tanks1), "activate",
                     GTK_SIGNAL_FUNC(on_select_nearby_tanks1_activate),
                     nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player1), "activate",
                     GTK_SIGNAL_FUNC(on_player1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player2), "activate",
                     GTK_SIGNAL_FUNC(on_player_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player3), "activate",
                     GTK_SIGNAL_FUNC(on_player3_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player4), "activate",
                     GTK_SIGNAL_FUNC(on_player4_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player5), "activate",
                     GTK_SIGNAL_FUNC(on_player5_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player6), "activate",
                     GTK_SIGNAL_FUNC(on_player6_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player7), "activate",
                     GTK_SIGNAL_FUNC(on_player7_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player8), "activate",
                     GTK_SIGNAL_FUNC(on_player8_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player9), "activate",
                     GTK_SIGNAL_FUNC(on_player9_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player10), "activate",
                     GTK_SIGNAL_FUNC(on_player10_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player11), "activate",
                     GTK_SIGNAL_FUNC(on_player11_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player12), "activate",
                     GTK_SIGNAL_FUNC(on_player12_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player13), "activate",
                     GTK_SIGNAL_FUNC(on_player13_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player14), "activate",
                     GTK_SIGNAL_FUNC(on_player14_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player15), "activate",
                     GTK_SIGNAL_FUNC(on_player15_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_player16), "activate",
                     GTK_SIGNAL_FUNC(on_player16_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(manual1), "activate",
                     GTK_SIGNAL_FUNC(on_manual1_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(help2), "activate",
                     GTK_SIGNAL_FUNC(on_help2_activate), nullptr);
  gtk_signal_connect(GTK_OBJECT(about1), "activate",
                     GTK_SIGNAL_FUNC(on_about1_activate), nullptr);

  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  gtk_widget_set_sensitive(idc_player1, FALSE);
  gtk_widget_set_sensitive(idc_player2, FALSE);
  gtk_widget_set_sensitive(idc_player3, FALSE);
  gtk_widget_set_sensitive(idc_player4, FALSE);
  gtk_widget_set_sensitive(idc_player5, FALSE);
  gtk_widget_set_sensitive(idc_player6, FALSE);
  gtk_widget_set_sensitive(idc_player7, FALSE);
  gtk_widget_set_sensitive(idc_player8, FALSE);
  gtk_widget_set_sensitive(idc_player9, FALSE);
  gtk_widget_set_sensitive(idc_player10, FALSE);
  gtk_widget_set_sensitive(idc_player11, FALSE);
  gtk_widget_set_sensitive(idc_player12, FALSE);
  gtk_widget_set_sensitive(idc_player13, FALSE);
  gtk_widget_set_sensitive(idc_player14, FALSE);
  gtk_widget_set_sensitive(idc_player15, FALSE);
  gtk_widget_set_sensitive(idc_player16, FALSE);
  gtk_widget_set_sensitive(window_size1, FALSE);
  gtk_widget_set_sensitive(request_alliance1, FALSE);
  gtk_widget_set_sensitive(leave_alliance1, FALSE);

  drawingarea1 = gtk_drawing_area_new();
  gtk_widget_ref(drawingarea1);
  gtk_object_set_data_full(GTK_OBJECT(window), "drawingarea1", drawingarea1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(drawingarea1);
  gtk_box_pack_start(GTK_BOX(vbox1), drawingarea1, TRUE, TRUE, 0);
  gtk_signal_connect(GTK_OBJECT(drawingarea1), "expose_event",
                     GTK_SIGNAL_FUNC(windowGetFocus), nullptr);

  /* Done */
}

/*********************************************************
 *NAME:          windowShowAllianceRequest
 *AUTHOR:        John Morrison
 *CREATION DATE: 03/02/03
 *LAST MODIFIED: 03/02/03
 *PURPOSE:
 * Sets whether the Allow Player Name Change menu item is
 * enabled
 *
 *ARGUMENTS:
 * allow - TRUE to enable/False to disable
 *********************************************************/
void windowAllowPlayerNameChange(bool allow) {
  gtk_widget_set_sensitive(change_player_name1, FALSE);
}

// FIXME
bool windowShowAllianceRequest() { return allowAlianceRequest; }
