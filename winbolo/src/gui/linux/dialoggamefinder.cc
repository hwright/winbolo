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

/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../bolo/global.h"
#include "../currentgames.h"
#include "../lang.h"
#include "../linresource.h"
#include "../netclient.h"
#include "dialogsetname.h"
#include "dialogtrackersetup.h"
#include "gamefront.h"
#include "messagebox.h"

#define EMPTY_STRING "\0"

static GtkWidget *dialogGameFindUs;
static bool useTracker = TRUE; /* Do we use the tracker for lookups */
static char motd[4 * 4096];
static currentGames cg = nullptr;
static GtkWidget *idc_address;
static GtkWidget *idc_gamefindport;
static GtkWidget *idc_gamefindversion;
static GtkWidget *idc_gamefindnumplayers;
static GtkWidget *idc_gamefindtype;
static GtkWidget *idc_gamefindbases;
static GtkWidget *idc_gamefindpills;
static GtkWidget *idc_gamefindmines;
static GtkWidget *idc_gamefindpass;
static GtkWidget *idc_gamefindbrains;
static GtkWidget *idc_gamefindstatus;
static GtkWidget *idc_gamefindjoin;
static GtkWidget *idc_gamefindrejoin;
static GtkWidget *list1;
static GtkWidget *button9;
static GtkWidget *button1;
static GtkWidget *button2;
static GtkWidget *button4;
static GtkWidget *button6;
static GtkWidget *button8;
static GtkWidget *button7;
static GtkWidget *idc_gamefindmapname;
static bool dialogGameFinderClosing = FALSE;

static gboolean dialogGameFinderSelect(GtkWidget *widget, gpointer user_data);
bool netClientFindBroadcastGames(GtkWidget *hWnd, currentGames *cg);
bool netClientFindTrackedGames(GtkWidget *hWnd, currentGames *cg,
                               char *trackerAddress, unsigned short port,
                               char *motd);

/*********************************************************
 *NAME:          dialogGameFinderClear
 *AUTHOR:        John Morrison
 *Creation Date: 18/1/00
 *Last Modified: 18/1/00
 *PURPOSE:
 * Clears the dialog of game information and empties the
 * list box.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void dialogGameFinderClear() {
  gtk_label_set_text(GTK_LABEL(idc_address), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindversion), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindport), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindnumplayers), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindtype), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindbases), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindpills), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindmines), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindpass), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindbrains), EMPTY_STRING);
  gtk_label_set_text(GTK_LABEL(idc_gamefindmapname), EMPTY_STRING);

  gtk_widget_set_sensitive(idc_gamefindjoin, FALSE);
  gtk_widget_set_sensitive(idc_gamefindrejoin, FALSE);

  gtk_list_clear_items(GTK_LIST(list1), 0, -1);
}

static gboolean dialogGameFinderCancel(GtkWidget *widget, GdkEventButton *event,
                                       gpointer user_data) {
  if (dialogGameFinderClosing == FALSE) {
    dialogGameFinderClosing = TRUE;
    gameFrontSetDlgState(dialogGameFindUs, openWelcome);
  }
  return FALSE;
}

/*********************************************************
 *NAME:          dialogGameFinderJoinTest
 *AUTHOR:        John Morrison
 *Creation Date: 18/1/00
 *Last Modified:  2/6/00
 *PURPOSE:
 * Called on attampt to join the currently selected game.
 * Returns whether we can join the game or not. ie version
 * numbers the same
 *
 *ARGUMENTS:
 *
 *********************************************************/
static bool dialogGameFinderJoinTest() {
  bool returnValue;                 /* Value to return */
  char address[FILENAME_MAX];       /* Address of the machine to join */
  char playerName[PLAYER_NAME_LEN]; /* Player Name */
  unsigned short port;              /* Port of the machine to join    */
  gchar *str;

  returnValue = TRUE;
  gtk_label_get(GTK_LABEL(idc_gamefindversion), &str);
  if (strncmp(str, STRVER, 4) != 0) {
    MessageBox(langGetText(STR_DLGGAMEFINDER_WRONGVERSION), DIALOG_BOX_TITLE);
    returnValue = FALSE;
  } else {
    gtk_label_get(GTK_LABEL(idc_gamefindport), &str);
    port = atoi(str);
    gtk_label_get(GTK_LABEL(idc_address), &str);
    strcpy(address, str);
    gameFrontGetPlayerName(playerName);
    gameFrontSetUdpOptions(playerName, address, port, 0);
  }
  return returnValue;
}

/* Check version */

static gboolean dialogGameFinderReJoin(GtkWidget *widget, GdkEventButton *event,
                                       gpointer user_data) {
  if (dialogGameFinderJoinTest() == TRUE) {
    gameFrontEnableRejoin();
    dialogGameFinderClosing = TRUE;
    gtk_widget_set_sensitive(dialogGameFindUs, FALSE);
    gdk_threads_leave();
    while (g_main_iteration(FALSE));
    gdk_threads_enter();
    if (gameFrontSetDlgState(dialogGameFindUs, openUdpJoin) == FALSE) {
      gtk_widget_set_sensitive(dialogGameFindUs, TRUE);
      dialogGameFinderClosing = FALSE;
    }
  }
  return FALSE;
}

static gboolean dialogGameFinderNew(GtkWidget *widget, GdkEventButton *event,
                                    gpointer user_data) {
  if (useTracker == TRUE) {
    gameFrontSetDlgState(dialogGameFindUs, openInternetSetup);
  } else {
    gameFrontSetDlgState(dialogGameFindUs, openLanSetup);
  }
  return FALSE;
}

static gboolean dialogGameFinderJoinByAddress(GtkWidget *widget,
                                              GdkEventButton *event,
                                              gpointer user_data) {
  char address[FILENAME_MAX];       /* Address of the machine to join */
  char playerName[PLAYER_NAME_LEN]; /* Player Name */
  unsigned short port;              /* Port of the machine to join    */

  gchar *str;
  gtk_label_get(GTK_LABEL(idc_gamefindport), &str);
  port = atoi(str);
  gtk_label_get(GTK_LABEL(idc_address), &str);
  strcpy(address, str);
  gameFrontGetPlayerName(playerName);
  gameFrontSetUdpOptions(playerName, address, port, 0);

  if (useTracker == TRUE) {
    gameFrontSetDlgState(dialogGameFindUs, openInternetManual);
  } else {
    gameFrontSetDlgState(dialogGameFindUs, openLanManual);
  }
  return FALSE;
}

static gboolean dialogGameFinderJoin(GtkWidget *widget, GdkEventButton *event,
                                     gpointer user_data) {
  if (dialogGameFinderJoinTest() == TRUE) {
    gtk_widget_set_sensitive(dialogGameFindUs, FALSE);
    gdk_threads_leave();
    while (g_main_iteration(FALSE));
    gdk_threads_enter();
    if (gameFrontSetDlgState(dialogGameFindUs, openUdpJoin) == FALSE) {
      gtk_widget_set_sensitive(dialogGameFindUs, TRUE);
    }
  }
  return FALSE;
}

static gboolean dialogGameFinderMOTD(GtkWidget *widget, GdkEventButton *event,
                                     gpointer user_data) {
  MessageBox(motd, langGetText(STR_DLGGAMEFINDER_MESSAGEOFTHEDAY));
  return FALSE;
}

static gboolean dialogGameFinderShow(GtkWidget *widget, GdkEventButton *event,
                                     gpointer user_data) {
  dialogGameFinderClosing = FALSE;
  dialogGameFinderClear();
  return FALSE;
}

/*********************************************************
 *NAME:          dialogGameFinderBuildList
 *AUTHOR:        John Morrison
 *Creation Date: 18/1/00
 *Last Modified: 23/1/00
 *PURPOSE:
 * Fills the list box with the current games
 *
 *ARGUMENTS:
 *  hWnd   - Handle to the window
 *********************************************************/
static void dialogGameFinderBuildList() {
  int count;                 /* Looping variable */
  int total;                 /* Total number of games found */
  char server[FILENAME_MAX]; /* Server name */
  GtkWidget *item;

  gtk_widget_set_sensitive(idc_gamefindjoin, FALSE);
  gtk_widget_set_sensitive(idc_gamefindrejoin, FALSE);
  total = currentGamesItemCount(&cg);
  if (total > 0) {
    count = 1;
    while (count <= total) {
      currentGamesGetServerName(&cg, count, server);
      item = gtk_list_item_new_with_label(server);
      gtk_signal_connect(GTK_OBJECT(item), "select",
                         GTK_SIGNAL_FUNC(dialogGameFinderSelect),
                         (gpointer)(size_t)count);
      gtk_container_add(GTK_CONTAINER(list1), item);
      gtk_widget_show(item);
      count++;
    }
  } else {
    /* Display no games found message */
    sprintf(server, "Status: %s",
            langGetText(STR_DLGGAMEFINDER_NOGAMESINPROGRESS));
    gtk_label_set_text(GTK_LABEL(idc_gamefindstatus), server);
  }
}

/*********************************************************
 *NAME:          dialogGameFinderFixMotd
 *AUTHOR:        John Morrison
 *Creation Date: 09/09/02
 *Last Modified: 09/09/02
 *PURPOSE:
 * Removes \r's from the motd string.
 *
 *ARGUMENTS:
 * motd - The message of the day string to remove \r's from
 **********************************************************/
static void dialogGameFinderFixMotd(char *motd) {
  int len; /* Length of the string */
  int pos; /* Our current position */

  len = strlen(motd);
  pos = len - 1;
  while (pos >= 0) {
    if (motd[pos] == '\r') {
      /* Remove it and recurse to find next */
      strcpy(motd + pos, motd + pos + 1);
      dialogGameFinderFixMotd(motd);
    }
    pos--;
  }
}

/*********************************************************
 *NAME:          dialogGameFinderSearch
 *AUTHOR:        John Morrison
 *Creation Date: 18/1/00
 *Last Modified:  4/6/00
 *PURPOSE:
 * Does the search for games.
 *
 *ARGUMENTS:
 *
 **********************************************************/
static void dialogGameFinderSearch() {
  char addr[FILENAME_MAX]; /* Tracker address */
  unsigned short port;     /* Tracker port    */
  bool dummy;              /* dummy variable  */
  bool ret;                /* Function return Value */

  if (useTracker == TRUE) {
    gameFrontGetTrackerOptions(addr, &port, &dummy);
    ret = netClientFindTrackedGames(idc_gamefindstatus, &cg, addr, port, motd);
    dialogGameFinderFixMotd(motd);
  } else {
    ret = netClientFindBroadcastGames(idc_gamefindstatus, &cg);
  }
  if (ret == TRUE) {
    sprintf(addr, "Status: %s", langGetText(STR_NETCLIENT_IDLE));
    gtk_label_set_text(GTK_LABEL(idc_gamefindstatus), addr);
    dialogGameFinderBuildList();
  }
}

/*********************************************************
 *NAME:          dialogGameFinderRefresh
 *AUTHOR:        John Morrison
 *Creation Date: 18/1/00
 *Last Modified: 18/1/00
 *PURPOSE:
 * Refreshs the list of active games.
 *
 *ARGUMENTS:
 *  hWnd   - Handle to the window
 *********************************************************/
static gboolean dialogGameFinderRefresh(GtkWidget *widget,
                                        GdkEventButton *event,
                                        gpointer user_data) {
  //  SetCursor(LoadCursor(NULL, IDC_WAIT));
  motd[0] = '\0';

  gtk_widget_set_sensitive(button4, FALSE);
  gtk_widget_set_sensitive(button2, FALSE);
  gtk_widget_set_sensitive(button7, FALSE);
  gtk_widget_set_sensitive(button6, FALSE);
  gtk_widget_set_sensitive(button1, FALSE);
  gtk_widget_set_sensitive(button9, FALSE);
  gtk_widget_set_sensitive(button8, FALSE);

  gdk_threads_leave();
  while (g_main_iteration(FALSE));
  gdk_threads_enter();

  dialogGameFinderClear();
  currentGamesDestroy(&cg);
  cg = currentGamesCreate();
  dialogGameFinderSearch();
  //  SetCursor(LoadCursor(NULL, IDC_ARROW));

  gtk_widget_set_sensitive(button4, TRUE);
  gtk_widget_set_sensitive(button2, TRUE);
  gtk_widget_set_sensitive(button7, TRUE);
  gtk_widget_set_sensitive(button6, TRUE);
  gtk_widget_set_sensitive(button8, TRUE);

  if (useTracker == TRUE) {
    gtk_widget_set_sensitive(button1, TRUE);
    gtk_widget_set_sensitive(button9, TRUE);
  }

  return FALSE;
}

static gboolean dialogGameFinderSelect(GtkWidget *widget, gpointer user_data) {
  char address[FILENAME_MAX];
  char str[FILENAME_MAX];
  char mapName[MAP_STR_SIZE];
  unsigned short port;
  BYTE numPlayers;
  BYTE numBases;
  BYTE numPills;
  bool mines;
  gameType game;
  aiType ai;
  bool password;
  int itemNum;

  itemNum = (size_t)user_data;
  currentGamesGetItem(&cg, itemNum, address, &port, mapName, str, &numPlayers,
                      &numBases, &numPills, &mines, &game, &ai, &password);
  /* Remove '\r''s */
  if (mapName[strlen(mapName) - 1] == '\r') {
    mapName[strlen(mapName) - 1] = EMPTY_CHAR;
  }
  if (str[strlen(str) - 1] == '\r') {
    str[strlen(str) - 1] = EMPTY_CHAR;
  }

  gtk_label_set_text(GTK_LABEL(idc_address), address);
  gtk_label_set_text(GTK_LABEL(idc_gamefindmapname), mapName);
  gtk_label_set_text(GTK_LABEL(idc_gamefindversion), str);

  sprintf(str, "%d", port);
  gtk_label_set_text(GTK_LABEL(idc_gamefindport), str);
  sprintf(str, "%d", numPlayers);
  gtk_label_set_text(GTK_LABEL(idc_gamefindnumplayers), str);
  sprintf(str, "%d", numBases);
  gtk_label_set_text(GTK_LABEL(idc_gamefindbases), str);
  sprintf(str, "%d", numPills);
  gtk_label_set_text(GTK_LABEL(idc_gamefindpills), str);
  if (mines == TRUE) {
    gtk_label_set_text(GTK_LABEL(idc_gamefindmines), langGetText(STR_YES));
  } else {
    gtk_label_set_text(GTK_LABEL(idc_gamefindmines), langGetText(STR_NO));
  }
  if (password == TRUE) {
    gtk_label_set_text(GTK_LABEL(idc_gamefindpass), langGetText(STR_YES));
  } else {
    gtk_label_set_text(GTK_LABEL(idc_gamefindpass), langGetText(STR_NO));
  }

  switch (ai) {
    case aiNone:
      gtk_label_set_text(GTK_LABEL(idc_gamefindbrains), langGetText(STR_NO));
      break;
    case aiYes:
      gtk_label_set_text(GTK_LABEL(idc_gamefindbrains), langGetText(STR_YES));
      break;
    default:
      gtk_label_set_text(GTK_LABEL(idc_gamefindbrains),
                         langGetText(STR_DLGGAMEFINDER_YESADV));
      break;
  }

  switch (game) {
    case gameOpen:

      gtk_label_set_text(GTK_LABEL(idc_gamefindtype),
                         langGetText(STR_DLGGAMEFINDER_OPEN));
      break;
    case gameTournament:
      gtk_label_set_text(GTK_LABEL(idc_gamefindtype),
                         langGetText(STR_DLGGAMEFINDER_TOURNAMENT));
      break;
    default:
      gtk_label_set_text(GTK_LABEL(idc_gamefindtype),
                         langGetText(STR_DLGGAMEFINDER_STRICTTOURNAMENT));
      break;
  }

  gtk_widget_set_sensitive(idc_gamefindjoin, TRUE);
  gtk_widget_set_sensitive(idc_gamefindrejoin, TRUE);

  return FALSE;
}

static gboolean dialogGameFinderSetName(GtkWidget *widget,
                                        GdkEventButton *event,
                                        gpointer user_data) {
  GtkWidget *g;

  g = dialogSetNameCreate(FALSE);
  gtk_widget_show(g);
  gtk_grab_add(g);
  gtk_main();

  return FALSE;
}
static gboolean dialogGameFinderTracker(GtkWidget *widget,
                                        GdkEventButton *event,
                                        gpointer user_data) {
  GtkWidget *tracker;

  tracker = dialogTrackerCreate();
  gtk_widget_show(tracker);
  gtk_grab_add(tracker);
  gtk_main();

  return FALSE;
}

GtkWidget *dialogGameFinderCreate(bool useTrack, char *title) {
  GtkWidget *dialogGameFinder;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *vbox2;
  GtkWidget *frame1;
  GtkWidget *table2;
  GtkWidget *label4;
  GtkWidget *label3;
  GtkWidget *label2;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label11;
  GtkWidget *label10;
  GtkWidget *label25;
  GtkWidget *table1;
  GtkWidget *label13;
  GtkWidget *label14;
  GtkWidget *label12;

  useTracker = useTrack;

  dialogGameFinder = gtk_dialog_new();
  gtk_object_set_data(GTK_OBJECT(dialogGameFinder), "dialogGameFinder",
                      dialogGameFinder);
  gtk_container_set_border_width(GTK_CONTAINER(dialogGameFinder), 15);
  gtk_window_set_title(GTK_WINDOW(dialogGameFinder), title);
  gtk_window_set_modal(GTK_WINDOW(dialogGameFinder), TRUE);
  gtk_window_set_position(GTK_WINDOW(dialogGameFinder), GTK_WIN_POS_CENTER);
  gtk_window_set_policy(GTK_WINDOW(dialogGameFinder), FALSE, FALSE, FALSE);
  vbox1 = gtk_dialog_get_content_area(GTK_DIALOG(dialogGameFinder));

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "hbox1", hbox1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 0);

  list1 = gtk_list_new();
  gtk_widget_ref(list1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "list1", list1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(list1);
  gtk_box_pack_start(GTK_BOX(hbox1), list1, TRUE, TRUE, 10);

  vbox2 = gtk_vbox_new(FALSE, 0);
  gtk_widget_ref(vbox2);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "vbox2", vbox2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(vbox2);
  gtk_box_pack_start(GTK_BOX(hbox1), vbox2, TRUE, TRUE, 0);

  frame1 = gtk_frame_new("Selected Game Information");
  gtk_widget_ref(frame1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "frame1", frame1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(frame1);
  gtk_box_pack_start(GTK_BOX(vbox2), frame1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(frame1), 7);

  table2 = gtk_table_new(11, 2, FALSE);
  gtk_widget_ref(table2);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "table2", table2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(table2);
  gtk_container_add(GTK_CONTAINER(frame1), table2);
  gtk_container_set_border_width(GTK_CONTAINER(table2), 7);

  label4 = gtk_label_new("Version: ");
  gtk_widget_ref(label4);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label4", label4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label4);
  gtk_table_attach(GTK_TABLE(table2), label4, 0, 1, 3, 4,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label4), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label4), 1, 0.5);

  label3 = gtk_label_new("Server Port: ");
  gtk_widget_ref(label3);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label3", label3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table2), label3, 0, 1, 1, 2,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label3), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label3), 1, 0.5);

  label2 = gtk_label_new("Server Address: ");
  gtk_widget_ref(label2);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label2", label2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label2);
  gtk_table_attach(GTK_TABLE(table2), label2, 0, 1, 0, 1,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label2), 1, 0.5);

  label5 = gtk_label_new("Number of Players:");
  gtk_widget_ref(label5);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label5", label5,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table2), label5, 0, 1, 4, 5,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label5), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label5), 0.5, 1);

  label6 = gtk_label_new("Game Type: ");
  gtk_widget_ref(label6);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label6", label6,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label6);
  gtk_table_attach(GTK_TABLE(table2), label6, 0, 1, 5, 6,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label6), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label6), 1, 0.5);

  label7 = gtk_label_new("No of free Bases: ");
  gtk_widget_ref(label7);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label7", label7,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label7);
  gtk_table_attach(GTK_TABLE(table2), label7, 0, 1, 6, 7,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label7), 1, 0.5);

  label8 = gtk_label_new("No of free Pillboxes: ");
  gtk_widget_ref(label8);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label8", label8,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label8);
  gtk_table_attach(GTK_TABLE(table2), label8, 0, 1, 7, 8,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label8), 1, 0.5);

  label9 = gtk_label_new("Hidden Mines: ");
  gtk_widget_ref(label9);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label9", label9,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label9);
  gtk_table_attach(GTK_TABLE(table2), label9, 0, 1, 8, 9,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label9), 1, 0.5);

  label11 = gtk_label_new("Brains: ");
  gtk_widget_ref(label11);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label11", label11,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label11);
  gtk_table_attach(GTK_TABLE(table2), label11, 0, 1, 10, 11,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label11), 1, 0.5);

  label10 = gtk_label_new("Password: ");
  gtk_widget_ref(label10);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label10", label10,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label10);
  gtk_table_attach(GTK_TABLE(table2), label10, 0, 1, 9, 10,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label10), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label10), 1, 0.5);

  idc_address = gtk_label_new("");
  gtk_widget_ref(idc_address);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_address",
                           idc_address, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_address);
  gtk_table_attach(GTK_TABLE(table2), idc_address, 1, 2, 0, 1,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(idc_address), GTK_JUSTIFY_LEFT);

  idc_gamefindport = gtk_label_new("");
  gtk_widget_ref(idc_gamefindport);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindport",
                           idc_gamefindport,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindport);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindport, 1, 2, 1, 2,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_gamefindversion = gtk_label_new("");
  gtk_widget_ref(idc_gamefindversion);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindversion",
                           idc_gamefindversion,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindversion);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindversion, 1, 2, 3, 4,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_gamefindnumplayers = gtk_label_new("");
  gtk_widget_ref(idc_gamefindnumplayers);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder),
                           "idc_gamefindnumplayers", idc_gamefindnumplayers,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindnumplayers);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindnumplayers, 1, 2, 4, 5,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_gamefindtype = gtk_label_new("");
  gtk_widget_ref(idc_gamefindtype);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindtype",
                           idc_gamefindtype,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindtype);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindtype, 1, 2, 5, 6,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_gamefindbases = gtk_label_new("");
  gtk_widget_ref(idc_gamefindbases);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindbases",
                           idc_gamefindbases,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindbases);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindbases, 1, 2, 6, 7,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(idc_gamefindbases), GTK_JUSTIFY_LEFT);

  idc_gamefindpills = gtk_label_new("");
  gtk_widget_ref(idc_gamefindpills);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindpills",
                           idc_gamefindpills,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindpills);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindpills, 1, 2, 7, 8,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(idc_gamefindpills), GTK_JUSTIFY_LEFT);

  idc_gamefindmines = gtk_label_new("");
  gtk_widget_ref(idc_gamefindmines);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindmines",
                           idc_gamefindmines,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindmines);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindmines, 1, 2, 8, 9,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(idc_gamefindmines), GTK_JUSTIFY_LEFT);

  idc_gamefindpass = gtk_label_new("");
  gtk_widget_ref(idc_gamefindpass);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindpass",
                           idc_gamefindpass,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindpass);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindpass, 1, 2, 9, 10,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(idc_gamefindpass), GTK_JUSTIFY_LEFT);

  idc_gamefindbrains = gtk_label_new("");
  gtk_widget_ref(idc_gamefindbrains);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindbrains",
                           idc_gamefindbrains,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindbrains);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindbrains, 1, 2, 10, 11,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(idc_gamefindbrains), GTK_JUSTIFY_LEFT);

  label25 = gtk_label_new("Map Name: ");
  gtk_widget_ref(label25);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label25", label25,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label25);
  gtk_table_attach(GTK_TABLE(table2), label25, 0, 1, 2, 3,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label25), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_alignment(GTK_MISC(label25), 1, 0.5);

  idc_gamefindmapname = gtk_label_new("");
  gtk_widget_ref(idc_gamefindmapname);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindmapname",
                           idc_gamefindmapname,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindmapname);
  gtk_table_attach(GTK_TABLE(table2), idc_gamefindmapname, 1, 2, 2, 3,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_gamefindstatus = gtk_label_new("Status: Idle");
  gtk_widget_ref(idc_gamefindstatus);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindstatus",
                           idc_gamefindstatus,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindstatus);
  gtk_box_pack_start(GTK_BOX(vbox2), idc_gamefindstatus, FALSE, TRUE, 3);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindstatus), 0.04, 0.5);

  table1 = gtk_table_new(3, 4, FALSE);
  gtk_widget_ref(table1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "table1", table1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 15);

  button2 = gtk_button_new_with_label("Join by Address");
  gtk_widget_ref(button2);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "button2", button2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button2);
  gtk_table_attach(GTK_TABLE(table1), button2, 2, 3, 0, 1,
                   (GtkAttachOptions)(0), (GtkAttachOptions)(0), 0, 0);

  button1 = gtk_button_new_with_label("Tracker Setup");
  gtk_widget_ref(button1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "button1", button1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button1);
  gtk_table_attach(GTK_TABLE(table1), button1, 0, 1, 0, 1,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  button8 = gtk_button_new_with_label("Set Player Name");
  gtk_widget_ref(button8);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "button8", button8,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button8);
  gtk_table_attach(GTK_TABLE(table1), button8, 0, 1, 1, 2,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  button9 = gtk_button_new_with_label("Message of the Day");
  gtk_widget_ref(button9);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "button9", button9,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button9);
  gtk_table_attach(GTK_TABLE(table1), button9, 0, 1, 2, 3,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  button4 = gtk_button_new_with_label("Refresh");
  gtk_widget_ref(button4);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "button4", button4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button4);
  gtk_table_attach(GTK_TABLE(table1), button4, 2, 3, 1, 2,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  button6 = gtk_button_new_with_label("New");
  gtk_widget_ref(button6);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "button6", button6,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button6);
  gtk_table_attach(GTK_TABLE(table1), button6, 2, 3, 2, 3,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  idc_gamefindjoin = gtk_button_new_with_label("Join");
  gtk_widget_ref(idc_gamefindjoin);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindjoin",
                           idc_gamefindjoin,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindjoin);
  gtk_table_attach(GTK_TABLE(table1), idc_gamefindjoin, 3, 4, 0, 1,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_widget_set_sensitive(idc_gamefindjoin, FALSE);

  idc_gamefindrejoin = gtk_button_new_with_label("Rejoin");
  gtk_widget_ref(idc_gamefindrejoin);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "idc_gamefindrejoin",
                           idc_gamefindrejoin,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamefindrejoin);
  gtk_table_attach(GTK_TABLE(table1), idc_gamefindrejoin, 3, 4, 1, 2,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_widget_set_sensitive(idc_gamefindrejoin, FALSE);

  button7 = gtk_button_new_with_label("Cancel");
  gtk_widget_ref(button7);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "button7", button7,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button7);
  gtk_table_attach(GTK_TABLE(table1), button7, 3, 4, 2, 3,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);

  label13 = gtk_label_new("");
  gtk_widget_ref(label13);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label13", label13,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label13);
  gtk_table_attach(GTK_TABLE(table1), label13, 1, 2, 1, 2,
                   (GtkAttachOptions)(0), (GtkAttachOptions)(0), 0, 0);

  label14 = gtk_label_new("");
  gtk_widget_ref(label14);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label14", label14,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label14);
  gtk_table_attach(GTK_TABLE(table1), label14, 1, 2, 2, 3,
                   (GtkAttachOptions)(0), (GtkAttachOptions)(0), 0, 0);

  label12 = gtk_label_new(
      "                                                            ");
  gtk_widget_ref(label12);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameFinder), "label12", label12,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label12);
  gtk_table_attach(GTK_TABLE(table1), label12, 1, 2, 0, 1,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);
  /* Signals */
  //  gtk_signal_connect (GTK_OBJECT(list1), "select",
  //  GTK_SIGNAL_FUNC(dialogGameFinderSelect), NULL);
  gtk_signal_connect(GTK_OBJECT(dialogGameFinder), "show",
                     GTK_SIGNAL_FUNC(dialogGameFinderShow), nullptr);
  gtk_signal_connect(GTK_OBJECT(button1), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderTracker), nullptr);
  gtk_signal_connect(GTK_OBJECT(button8), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderSetName), nullptr);
  gtk_signal_connect(GTK_OBJECT(button4), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderRefresh), nullptr);
  gtk_signal_connect(GTK_OBJECT(button7), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderCancel), nullptr);
  gtk_signal_connect(GTK_OBJECT(button9), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderMOTD), nullptr);
  gtk_signal_connect(GTK_OBJECT(dialogGameFinder), "delete_event",
                     GTK_SIGNAL_FUNC(dialogGameFinderCancel), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamefindrejoin), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderReJoin), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamefindjoin), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderJoin), nullptr);
  gtk_signal_connect(GTK_OBJECT(button2), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderJoinByAddress), nullptr);
  gtk_signal_connect(GTK_OBJECT(button6), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameFinderNew), nullptr);
  /* My Stuff */
  cg = currentGamesCreate();
  if (useTracker == TRUE) {
    strcpy(motd, langGetText(STR_DLGGAMEFINDER_REFRESHFIRST));
  } else {
    gtk_widget_set_sensitive(button1, FALSE);
    gtk_widget_set_sensitive(button9, FALSE);
  }

  dialogGameFindUs = dialogGameFinder;
  gtk_misc_set_alignment(GTK_MISC(idc_address), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindport), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindversion), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindnumplayers), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindtype), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindbases), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindmines), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindmapname), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindpills), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindpass), 0, 0.5);
  gtk_misc_set_alignment(GTK_MISC(idc_gamefindbrains), 0, 0.5);

  return dialogGameFinder;
}
