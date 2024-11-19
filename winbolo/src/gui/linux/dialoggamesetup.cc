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

#include <fstream>

#include "../../bolo/global.h"
#include "../../bolo/util.h"
#include "../lang.h"
#include "../linresource.h"
#include "gamefront.h"
#include "messagebox.h"

/* There are 60 seconds in a minute */
#define NUM_SECONDS 60

static GtkWidget *idc_gamesetupstartdelay;
static GtkWidget *idc_gamesetuptxtstartdelay;
static GtkWidget *idc_gamesetupradio1;
static GtkWidget *idc_gamesetupradio2;
static GtkWidget *idc_gamesetupradio3;
static GtkWidget *idc_gamesetuphiddenmines;
static GtkWidget *idc_gamesetupcomptanks1;
static GtkWidget *idc_gamesetupcomptanks2;
static GtkWidget *idc_gamesetupselectedmap;
static GtkWidget *idc_gamesetupgamepassword;
static GtkWidget *idc_gamesetuptextpassword;
static GtkWidget *idc_gamesetuptimelimit;
static GtkWidget *idc_gamesetuptxtlimit;
static GtkWidget *filew;
static GtkWidget *dialogGameSetupUs;

static bool closing = FALSE;
static char
    fileName[FILENAME_MAX]; /* The filename and path that should be opened */

static void file_cancel_sel(GtkWidget *w, GtkFileSelection *fs) {
  gtk_widget_destroy(filew);
}

static void file_ok_sel(GtkWidget *w, GtkFileSelection *fs) {
  gchar *sTempFile;
  sTempFile = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fs));

  strcpy(fileName, sTempFile);
  gtk_widget_destroy(filew);
}

static int DestroyDialog(GtkWidget *widget, gpointer *data) {
  gtk_grab_remove(widget);
  gtk_main_quit();
  return (FALSE);
}

/*********************************************************
 *NAME:          dialogGameSetupMapCheck
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/1/99
 *LAST MODIFIED: 12/12/99
 *PURPOSE:
 * Checks to see if the map is valid set the Map name
 * to be it and enable the dialog OK button -
 * else disable the OK button
 *
 *ARGUMENTS:
 *  fileName - filename to check
 *********************************************************/
static void dialogGameSetupMapCheck(char *fileName) {
  char output[FILENAME_MAX];  /* String to output */
  bool openOK;                /* Was the map opened OK? */

  openOK = TRUE;
  output[0] = '\0';
  std::ifstream input(fileName);
  std::string mapName = bolo::utilExtractMapName(fileName);
  openOK = screenLoadMap(input, mapName.c_str(), gameOpen, FALSE, 0,
                         UNLIMITED_GAME_TIME,
                         langGetText(STR_DLGGAMESETUP_DEFAULTNAME), TRUE);
  if (openOK == FALSE) {
    MessageBox(langGetText(STR_DLGGAMESETUP_ERROROPENINGMAP), DIALOG_BOX_TITLE);
    gtk_label_set_text(GTK_LABEL(idc_gamesetupselectedmap),
                       langGetText(STR_DLGGAMESETUP_SELECTEDMAPINBUILT));
    gameFrontSetFileName("");
  } else {
    char mapName[FILENAME_MAX]; /* Name of the map */
    mapName[0] = '\0';
    screenGetMapName(mapName);
    strcpy(output, langGetText(STR_DLGGAMESETUP_SELECTEDMAP));
    strcat(output, mapName);
    gameFrontSetFileName(fileName);
    gtk_label_set_text(GTK_LABEL(idc_gamesetupselectedmap), output);
  }
}

static gboolean dialogGameSetupChooseMap(GtkWidget *widget,
                                         GdkEventButton *event,
                                         gpointer user_data) {
  fileName[0] = '\0';
  filew = gtk_file_selection_new("Open Map File...");

  gtk_signal_connect(GTK_OBJECT(filew), "destroy", (GtkSignalFunc)DestroyDialog,
                     &filew);

  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
                     "clicked", (GtkSignalFunc)file_ok_sel, filew);
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filew)->cancel_button),
                     "clicked", (GtkSignalFunc)file_cancel_sel, filew);
  gtk_widget_show(filew);

  /* --- Make sure we keep the focus --- */
  gtk_grab_add(filew);
  gtk_main();
  if (fileName[0] != '\0') {
    dialogGameSetupMapCheck(fileName);
  }
  return FALSE;
}

static gboolean dialogGameSetupPassword(GtkWidget *widget,
                                        GdkEventButton *event,
                                        gpointer user_data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) {
    gtk_widget_set_sensitive(idc_gamesetuptextpassword, TRUE);
  } else {
    gtk_widget_set_sensitive(idc_gamesetuptextpassword, FALSE);
  }
  return FALSE;
}

static gboolean dialogGameSetupCompTanks(GtkWidget *widget,
                                         GdkEventButton *event,
                                         gpointer user_data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) {
    gtk_widget_set_sensitive(idc_gamesetupcomptanks2, TRUE);
  } else {
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks2),
                                FALSE);
    gtk_widget_set_sensitive(idc_gamesetupcomptanks2, FALSE);
  }
  return FALSE;
}

static gboolean dialogGameSetupTimeLimit(GtkWidget *widget,
                                         GdkEventButton *event,
                                         gpointer user_data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) {
    gtk_widget_set_sensitive(idc_gamesetuptxtlimit, TRUE);
  } else {
    gtk_widget_set_sensitive(idc_gamesetuptxtlimit, FALSE);
  }
  return FALSE;
}
static gboolean dialogGameSetupStartDelay(GtkWidget *widget,
                                          GdkEventButton *event,
                                          gpointer user_data) {
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)) == TRUE) {
    gtk_widget_set_sensitive(idc_gamesetuptxtstartdelay, TRUE);
  } else {
    gtk_widget_set_sensitive(idc_gamesetuptxtstartdelay, FALSE);
  }
  return FALSE;
}

static gboolean dialogGameSetupShow(GtkWidget *widget, GdkEventButton *event,
                                    gpointer user_data) {
  char password[FILENAME_MAX]; /* Dialog Options               */
  gameType gt;
  bool hm;
  aiType ai;
  long sd;
  long tlimit;

  closing = FALSE;
  gameFrontGetGameOptions(password, &gt, &hm, &ai, &sd, &tlimit);
  if (password[0] != '\0') {
    gtk_entry_set_text(GTK_ENTRY(idc_gamesetuptextpassword), password);
    gtk_widget_set_sensitive(idc_gamesetuptextpassword, TRUE);
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupgamepassword),
                                TRUE);
  } else {
    gtk_entry_set_text(GTK_ENTRY(idc_gamesetuptextpassword), "");
    gtk_widget_set_sensitive(idc_gamesetuptextpassword, FALSE);
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupgamepassword),
                                FALSE);
  }

  switch (gt) {
    case gameOpen:
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupradio1), TRUE);
      break;
    case gameTournament:
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupradio2), TRUE);
      break;
    default:
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupradio3), TRUE);
      break;
  }

  if (hm == TRUE) {
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetuphiddenmines),
                                TRUE);
  } else {
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetuphiddenmines),
                                FALSE);
  }
  switch (ai) {
    case aiYesAdvantage:
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks1),
                                  TRUE);
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks2),
                                  TRUE);
      gtk_widget_set_sensitive(idc_gamesetupcomptanks2, TRUE);
      break;
    case aiYes:
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks1),
                                  TRUE);
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks2),
                                  FALSE);
      gtk_widget_set_sensitive(idc_gamesetupcomptanks2, FALSE);
      break;
    default:
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks1),
                                  FALSE);
      gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks2),
                                  FALSE);
      gtk_widget_set_sensitive(idc_gamesetupcomptanks2, FALSE);

      break;
  }

  if (tlimit != UNLIMITED_GAME_TIME && tlimit > 0) {
    tlimit /= GAME_NUMGAMETICKS_SEC;
    tlimit /= NUM_SECONDS;
    sprintf(password, "%ld", tlimit);
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetuptimelimit),
                                TRUE);
    gtk_widget_set_sensitive(idc_gamesetuptxtlimit, TRUE);
    gtk_entry_set_text(GTK_ENTRY(idc_gamesetuptxtlimit), password);
  } else {
    gtk_widget_set_sensitive(idc_gamesetuptxtlimit, FALSE);
    gtk_entry_set_text(GTK_ENTRY(idc_gamesetuptxtlimit), "");
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetuptimelimit),
                                FALSE);
  }

  if (sd > 0) {
    sd /= GAME_NUMGAMETICKS_SEC;
    sprintf(password, "%ld", sd);
    gtk_widget_set_sensitive(idc_gamesetuptxtstartdelay, TRUE);
    gtk_entry_set_text(GTK_ENTRY(idc_gamesetuptxtstartdelay), password);
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupstartdelay),
                                TRUE);

  } else {
    gtk_widget_set_sensitive(idc_gamesetuptxtstartdelay, FALSE);
    gtk_entry_set_text(GTK_ENTRY(idc_gamesetuptxtstartdelay), "");
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(idc_gamesetupstartdelay),
                                FALSE);
  }
  gtk_label_set_text(GTK_LABEL(idc_gamesetupselectedmap),
                     "Selected Map: Everard Island (inbuilt)");
  return FALSE;
}

static gboolean dialogGameSetupOK(GtkWidget *widget, GdkEventButton *event,
                                  gpointer user_data) {
  gameType gt;            /* Game type */
  bool hm;                /* Hidden Mines */
  aiType ai;              /* AI Type */
  long gsd;               /* Game start delay */
  long gl;                /* Game length */
  char str[MAP_STR_SIZE]; /* Holds password and used to temp hold strings before
                             they turn into numbers */

  closing = TRUE;
  str[0] = '\0';
  gsd = 0;
  gl = UNLIMITED_GAME_TIME;

  /* Game type */
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_gamesetupradio1)) ==
      TRUE) {
    gt = gameOpen;
  } else if (gtk_toggle_button_get_active(
                 GTK_TOGGLE_BUTTON(idc_gamesetupradio2)) == TRUE) {
    gt = gameTournament;
  } else {
    gt = gameStrictTournament;
  }

  /* Hidden Mines */
  if (gtk_toggle_button_get_active(
          GTK_TOGGLE_BUTTON(idc_gamesetuphiddenmines)) == TRUE) {
    hm = TRUE;
  } else {
    hm = FALSE;
  }

  /* Allow AI */
  if (gtk_toggle_button_get_active(
          GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks1)) == TRUE) {
    if (gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(idc_gamesetupcomptanks2)) == TRUE) {
      ai = aiYesAdvantage;
    } else {
      ai = aiYes;
    }
  } else {
    ai = aiNone;
  }

  /* Game Start Delay */
  if (gtk_toggle_button_get_active(
          GTK_TOGGLE_BUTTON(idc_gamesetupstartdelay)) == TRUE) {
    gchar *str2 = gtk_entry_get_text(GTK_ENTRY(idc_gamesetuptxtstartdelay));
    strcpy(str, str2);
    gsd = atol(str);
    if (gsd < 0) {
      gsd = 0;
    }
  }

  /* Get Game Len */
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_gamesetuptimelimit)) ==
      TRUE) {
    gchar *str2 = gtk_entry_get_text(GTK_ENTRY(idc_gamesetuptxtlimit));
    strcpy(str, str2);
    gl = atol(str);
    if (gl < 0) {
      gl = UNLIMITED_GAME_TIME;
    }
  }

  /* Get Password */
  str[0] = '\0';
  if (gtk_toggle_button_get_active(
          GTK_TOGGLE_BUTTON(idc_gamesetupgamepassword)) == TRUE) {
    gchar *str2 = gtk_entry_get_text(GTK_ENTRY(idc_gamesetuptextpassword));
    strcpy(str, str2);
  }

  /* Change Game Start delay from seconds to ticks */
  if (gsd > 0) {
    gsd *= GAME_NUMGAMETICKS_SEC;
  }

  /* Change Game Length from minutes to ticks */
  if (gl > 0) {
    gl *= NUM_SECONDS;
    gl *= GAME_NUMGAMETICKS_SEC;
  }

  /* Pass values back */
  gameFrontSetGameOptions(str, gt, hm, ai, gsd, gl, FALSE);
  gtk_widget_set_sensitive(dialogGameSetupUs, FALSE);
  if (gameFrontSetDlgState(dialogGameSetupUs, openFinished) == FALSE) {
    closing = FALSE;
    gtk_widget_set_sensitive(dialogGameSetupUs, TRUE);
  }
  return FALSE;
}

static gboolean dialogGameSetupCancel(GtkWidget *widget, GdkEventButton *event,
                                      gpointer user_data) {
  if (closing == FALSE) {
    closing = TRUE;
    gameFrontSetDlgState(dialogGameSetupUs, openWelcome);
  }
  return FALSE;
}

GtkWidget *dialogGameSetupCreate(void) {
  GtkWidget *dialogGameSetup;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *hbox1;
  GtkWidget *idc_gamesetupchoosemap;
  GSList *vbox1_group = nullptr;
  GtkWidget *hbox2;
  GtkWidget *hbox3;
  GtkWidget *hbox4;
  GtkWidget *label3;
  GtkWidget *hbox5;
  GtkWidget *label4;
  GtkWidget *hbox6;
  GtkWidget *idc_gamesetupok;
  GtkWidget *idc_gamesetupcancel;

  dialogGameSetup = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data(GTK_OBJECT(dialogGameSetup), "dialogGameSetup",
                      dialogGameSetup);
  gtk_window_set_policy(GTK_WINDOW(dialogGameSetup), FALSE, FALSE, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(dialogGameSetup), 15);
  gtk_window_set_title(GTK_WINDOW(dialogGameSetup), "Game Setup");

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_ref(vbox1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "vbox1", vbox1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(vbox1);
  gtk_container_add(GTK_CONTAINER(dialogGameSetup), vbox1);

  label1 =
      gtk_label_new("Please select the options you want for your new game:");
  gtk_widget_ref(label1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "label1", label1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label1);
  gtk_misc_set_alignment(GTK_MISC(label1), 0.0, 0.5);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_LEFT);

  hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "hbox1", hbox1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox1);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox1, TRUE, TRUE, 9);

  idc_gamesetupchoosemap = gtk_button_new_with_label("Choose map");
  gtk_widget_ref(idc_gamesetupchoosemap);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup),
                           "idc_gamesetupchoosemap", idc_gamesetupchoosemap,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupchoosemap);
  gtk_box_pack_start(GTK_BOX(hbox1), idc_gamesetupchoosemap, FALSE, FALSE, 0);

  idc_gamesetupselectedmap =
      gtk_label_new("Selected Map: Everard Island (Inbuilt)");
  gtk_widget_ref(idc_gamesetupselectedmap);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup),
                           "idc_gamesetupselectedmap", idc_gamesetupselectedmap,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupselectedmap);
  gtk_box_pack_start(GTK_BOX(hbox1), idc_gamesetupselectedmap, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(idc_gamesetupselectedmap), GTK_JUSTIFY_LEFT);
  gtk_misc_set_padding(GTK_MISC(idc_gamesetupselectedmap), 10, 0);

  idc_gamesetupradio1 = gtk_radio_button_new_with_label(
      vbox1_group, "Open Game (each tank comes pre-armed)");
  vbox1_group = gtk_radio_button_group(GTK_RADIO_BUTTON(idc_gamesetupradio1));
  gtk_widget_ref(idc_gamesetupradio1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "idc_gamesetupradio1",
                           idc_gamesetupradio1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupradio1);
  gtk_box_pack_start(GTK_BOX(vbox1), idc_gamesetupradio1, FALSE, FALSE, 0);

  idc_gamesetupradio2 = gtk_radio_button_new_with_label(
      vbox1_group, "Tournament (free ammunition early in the game)");
  vbox1_group = gtk_radio_button_group(GTK_RADIO_BUTTON(idc_gamesetupradio2));
  gtk_widget_ref(idc_gamesetupradio2);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "idc_gamesetupradio2",
                           idc_gamesetupradio2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupradio2);
  gtk_box_pack_start(GTK_BOX(vbox1), idc_gamesetupradio2, FALSE, FALSE, 0);

  idc_gamesetupradio3 = gtk_radio_button_new_with_label(
      vbox1_group, "Strict Tournament (no free ammunition at all)");
  vbox1_group = gtk_radio_button_group(GTK_RADIO_BUTTON(idc_gamesetupradio3));
  gtk_widget_ref(idc_gamesetupradio3);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "idc_gamesetupradio3",
                           idc_gamesetupradio3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupradio3);
  gtk_box_pack_start(GTK_BOX(vbox1), idc_gamesetupradio3, FALSE, FALSE, 0);

  idc_gamesetuphiddenmines =
      gtk_check_button_new_with_label("Allow Hidden Mines");
  gtk_widget_ref(idc_gamesetuphiddenmines);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup),
                           "idc_gamesetuphiddenmines", idc_gamesetuphiddenmines,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetuphiddenmines);
  gtk_box_pack_start(GTK_BOX(vbox1), idc_gamesetuphiddenmines, FALSE, FALSE, 3);

  hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox2);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "hbox2", hbox2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox2);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox2, FALSE, FALSE, 0);

  idc_gamesetupcomptanks1 =
      gtk_check_button_new_with_label("Allow Computer Tanks");
  gtk_widget_ref(idc_gamesetupcomptanks1);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup),
                           "idc_gamesetupcomptanks1", idc_gamesetupcomptanks1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupcomptanks1);
  gtk_box_pack_start(GTK_BOX(hbox2), idc_gamesetupcomptanks1, FALSE, FALSE, 0);

  idc_gamesetupcomptanks2 =
      gtk_check_button_new_with_label("and give them an advantage");
  gtk_widget_ref(idc_gamesetupcomptanks2);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup),
                           "idc_gamesetupcomptanks2", idc_gamesetupcomptanks2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_set_sensitive(idc_gamesetupcomptanks2, FALSE);
  gtk_widget_show(idc_gamesetupcomptanks2);
  gtk_box_pack_start(GTK_BOX(hbox2), idc_gamesetupcomptanks2, FALSE, FALSE, 0);

  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox3);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "hbox3", hbox3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox3, TRUE, TRUE, 2);

  idc_gamesetupgamepassword = gtk_check_button_new_with_label("Game Password");
  gtk_widget_ref(idc_gamesetupgamepassword);
  gtk_object_set_data_full(
      GTK_OBJECT(dialogGameSetup), "idc_gamesetupgamepassword",
      idc_gamesetupgamepassword, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupgamepassword);
  gtk_box_pack_start(GTK_BOX(hbox3), idc_gamesetupgamepassword, FALSE, FALSE,
                     0);

  idc_gamesetuptextpassword = gtk_entry_new();
  gtk_widget_ref(idc_gamesetuptextpassword);
  gtk_object_set_data_full(
      GTK_OBJECT(dialogGameSetup), "idc_gamesetuptextpassword",
      idc_gamesetuptextpassword, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetuptextpassword);
  gtk_box_pack_start(GTK_BOX(hbox3), idc_gamesetuptextpassword, TRUE, TRUE, 0);
  gtk_widget_set_sensitive(idc_gamesetuptextpassword, FALSE);
  gtk_entry_set_visibility(GTK_ENTRY(idc_gamesetuptextpassword), FALSE);

  hbox4 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox4);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "hbox4", hbox4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox4);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox4, TRUE, TRUE, 5);

  idc_gamesetupstartdelay = gtk_check_button_new_with_label("Game start delay");
  gtk_widget_ref(idc_gamesetupstartdelay);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup),
                           "idc_gamesetupstartdelay", idc_gamesetupstartdelay,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupstartdelay);
  gtk_box_pack_start(GTK_BOX(hbox4), idc_gamesetupstartdelay, FALSE, FALSE, 0);

  idc_gamesetuptxtstartdelay = gtk_entry_new();
  gtk_widget_ref(idc_gamesetuptxtstartdelay);
  gtk_object_set_data_full(
      GTK_OBJECT(dialogGameSetup), "idc_gamesetuptxtstartdelay",
      idc_gamesetuptxtstartdelay, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetuptxtstartdelay);
  gtk_box_pack_start(GTK_BOX(hbox4), idc_gamesetuptxtstartdelay, TRUE, TRUE, 0);
  gtk_widget_set_sensitive(idc_gamesetuptxtstartdelay, FALSE);

  label3 = gtk_label_new("seconds");
  gtk_widget_ref(label3);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "label3", label3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label3);
  gtk_box_pack_start(GTK_BOX(hbox4), label3, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label3), 10, 0);

  hbox5 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox5);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "hbox5", hbox5,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox5);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox5, TRUE, TRUE, 0);

  idc_gamesetuptimelimit = gtk_check_button_new_with_label("Game time limit");
  gtk_widget_ref(idc_gamesetuptimelimit);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup),
                           "idc_gamesetuptimelimit", idc_gamesetuptimelimit,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetuptimelimit);
  gtk_box_pack_start(GTK_BOX(hbox5), idc_gamesetuptimelimit, FALSE, FALSE, 0);

  idc_gamesetuptxtlimit = gtk_entry_new();
  gtk_widget_ref(idc_gamesetuptxtlimit);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "idc_gamesetuptxtlimit",
                           idc_gamesetuptxtlimit,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetuptxtlimit);
  gtk_box_pack_start(GTK_BOX(hbox5), idc_gamesetuptxtlimit, TRUE, TRUE, 0);
  gtk_widget_set_sensitive(idc_gamesetuptxtlimit, FALSE);

  label4 = gtk_label_new("minutes");
  gtk_widget_ref(label4);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "label4", label4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label4);
  gtk_box_pack_start(GTK_BOX(hbox5), label4, FALSE, FALSE, 0);
  gtk_misc_set_padding(GTK_MISC(label4), 10, 0);

  hbox6 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox6);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "hbox6", hbox6,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox6);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox6, TRUE, TRUE, 10);

  idc_gamesetupok = gtk_button_new_with_label("OK");
  gtk_widget_ref(idc_gamesetupok);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "idc_gamesetupok",
                           idc_gamesetupok, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupok);
  gtk_box_pack_start(GTK_BOX(hbox6), idc_gamesetupok, TRUE, TRUE, 20);

  idc_gamesetupcancel = gtk_button_new_with_label("Cancel");
  gtk_widget_ref(idc_gamesetupcancel);
  gtk_object_set_data_full(GTK_OBJECT(dialogGameSetup), "idc_gamesetupcancel",
                           idc_gamesetupcancel,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_gamesetupcancel);
  gtk_box_pack_start(GTK_BOX(hbox6), idc_gamesetupcancel, TRUE, TRUE, 20);

  gtk_signal_connect(GTK_OBJECT(idc_gamesetupcancel), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameSetupCancel), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamesetupok), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameSetupOK), nullptr);
  gtk_signal_connect(GTK_OBJECT(dialogGameSetup), "delete_event",
                     GTK_SIGNAL_FUNC(dialogGameSetupCancel), nullptr);
  gtk_signal_connect(GTK_OBJECT(dialogGameSetup), "show",
                     GTK_SIGNAL_FUNC(dialogGameSetupShow), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamesetupgamepassword), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameSetupPassword), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamesetuptimelimit), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameSetupTimeLimit), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamesetupstartdelay), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameSetupStartDelay), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamesetupcomptanks1), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameSetupCompTanks), nullptr);
  gtk_signal_connect(GTK_OBJECT(idc_gamesetupchoosemap), "clicked",
                     GTK_SIGNAL_FUNC(dialogGameSetupChooseMap), nullptr);

  gtk_window_set_position(GTK_WINDOW(dialogGameSetup), GTK_WIN_POS_CENTER);
  dialogGameSetupUs = dialogGameSetup;

  return dialogGameSetup;
}
