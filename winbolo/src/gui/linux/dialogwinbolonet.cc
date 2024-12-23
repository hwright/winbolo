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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../bolo/global.h"
#include "messagebox.h"

static GtkWidget *dialogWbnUs;
static GtkWidget *dialogWbnUse;
static GtkWidget *dialogWbnSavePass;
static GtkWidget *dialogWbnPassword;

void gameFrontGetPlayerName(char *pn);

static void dialogWinboloNetCloseBox(GtkWidget *widget, gpointer user_data) {
  gtk_widget_destroy(dialogWbnUs);
  gtk_main_quit();
}

void gameFrontSetWinbolonetSettings(char *password, bool useWbn, bool savePass);
void gameFrontGetWinbolonetSettings(char *password, bool *useWbn,
                                    bool *savePass);

static void dialogWinbolonetOK(GtkWidget *widget, gpointer user_data) {
  char password[FILENAME_MAX];
  bool useWbn = FALSE;
  bool savePass = FALSE;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dialogWbnUse)) == TRUE) {
    useWbn = TRUE;
  }
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(dialogWbnSavePass)) ==
      TRUE) {
    savePass = TRUE;
  }

  const gchar *str = gtk_entry_get_text(GTK_ENTRY(dialogWbnPassword));
  strcpy(password, str);
  if (password[0] == EMPTY_CHAR && useWbn == TRUE) {
    MessageBox(
        "Sorry, you must enter password if you wish to participate in "
        "WinBolo.net",
        DIALOG_BOX_TITLE);
  } else {
    gameFrontSetWinbolonetSettings(password, useWbn, savePass);
    dialogWinboloNetCloseBox(widget, user_data);
  }
}

GtkWidget *dialogWinboloNetCreate() {
  GtkWidget *Winbolo_net_settings;
  GtkWidget *dialog_vbox1;
  GtkWidget *vbox1;
  GtkWidget *hbox4;
  GtkWidget *label2;
  GtkWidget *password;
  GtkWidget *checkbutton1;
  GtkWidget *checkbutton2;
  GtkWidget *label1;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbox3;
  GtkWidget *button3;
  GtkWidget *button4;
  char userName[FILENAME_MAX];
  char passwordStr[FILENAME_MAX];
  bool useWbn;
  bool savePass;
  gameFrontGetPlayerName(userName);
  gameFrontGetWinbolonetSettings(passwordStr, &useWbn, &savePass);

  Winbolo_net_settings = gtk_dialog_new();
  gtk_object_set_data(GTK_OBJECT(Winbolo_net_settings), "Winbolo_net_settings",
                      Winbolo_net_settings);
  gtk_container_set_border_width(GTK_CONTAINER(Winbolo_net_settings), 10);
  gtk_window_set_title(GTK_WINDOW(Winbolo_net_settings),
                       "Winbolo.net settings");
  gtk_window_set_policy(GTK_WINDOW(Winbolo_net_settings), FALSE, FALSE, FALSE);
  gtk_window_set_position(GTK_WINDOW(Winbolo_net_settings), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(Winbolo_net_settings), TRUE);

  dialog_vbox1 = gtk_dialog_get_content_area(GTK_DIALOG(Winbolo_net_settings));
  gtk_object_set_data(GTK_OBJECT(Winbolo_net_settings), "dialog_vbox1",
                      dialog_vbox1);
  gtk_widget_show(dialog_vbox1);

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_ref(vbox1);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "vbox1", vbox1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(vbox1);
  gtk_box_pack_start(GTK_BOX(dialog_vbox1), vbox1, TRUE, TRUE, 0);

  hbox4 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox4);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "hbox4", hbox4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox4);
  gtk_box_pack_start(GTK_BOX(vbox1), hbox4, TRUE, TRUE, 7);

  label2 = gtk_label_new("Password: ");
  gtk_widget_ref(label2);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "label2", label2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label2);
  gtk_box_pack_start(GTK_BOX(hbox4), label2, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_RIGHT);

  password = gtk_entry_new_with_max_length(32);
  gtk_entry_set_text(GTK_ENTRY(password), passwordStr);
  gtk_widget_ref(password);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "password",
                           password, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(password);
  gtk_box_pack_start(GTK_BOX(hbox4), password, TRUE, TRUE, 0);
  gtk_entry_set_visibility(GTK_ENTRY(password), FALSE);
  dialogWbnPassword = password;

  checkbutton1 = gtk_check_button_new_with_label("Use Winbolo.net");
  gtk_widget_ref(checkbutton1);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "checkbutton1",
                           checkbutton1, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(checkbutton1);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton1), useWbn);
  dialogWbnUse = checkbutton1;
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton1, FALSE, FALSE, 0);

  checkbutton2 =
      gtk_check_button_new_with_label("Save My Winbolo.net Password");
  gtk_widget_ref(checkbutton2);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "checkbutton2",
                           checkbutton2, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(checkbutton2);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton2), savePass);
  dialogWbnSavePass = checkbutton2;
  gtk_box_pack_start(GTK_BOX(vbox1), checkbutton2, FALSE, FALSE, 0);

  label1 = gtk_label_new(
      "Winbolo.net is a free real time game tracking and player statisitics "
      "website. To signup or for more informaton please visit "
      "http://www.winbolo.net");
  gtk_widget_ref(label1);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "label1", label1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_line_wrap(GTK_LABEL(label1), TRUE);
  gtk_misc_set_padding(GTK_MISC(label1), 0, 23);

  dialog_action_area1 = GTK_DIALOG(Winbolo_net_settings)->action_area;
  gtk_object_set_data(GTK_OBJECT(Winbolo_net_settings), "dialog_action_area1",
                      dialog_action_area1);
  gtk_widget_show(dialog_action_area1);
  gtk_container_set_border_width(GTK_CONTAINER(dialog_action_area1), 10);

  hbox3 = gtk_hbox_new(FALSE, 0);
  gtk_widget_ref(hbox3);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "hbox3", hbox3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(hbox3);
  gtk_box_pack_start(GTK_BOX(dialog_action_area1), hbox3, TRUE, TRUE, 111);
  gtk_container_set_border_width(GTK_CONTAINER(hbox3), 3);

  button3 = gtk_button_new_with_label("OK");
  gtk_widget_ref(button3);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "button3", button3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button3);
  gtk_box_pack_start(GTK_BOX(hbox3), button3, FALSE, FALSE, 0);

  button4 = gtk_button_new_with_label("Cancel");
  gtk_widget_ref(button4);
  gtk_object_set_data_full(GTK_OBJECT(Winbolo_net_settings), "button4", button4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button4);
  gtk_box_pack_start(GTK_BOX(hbox3), button4, FALSE, FALSE, 0);

  gtk_signal_connect(GTK_OBJECT(Winbolo_net_settings), "delete_event",
                     GTK_SIGNAL_FUNC(dialogWinboloNetCloseBox), nullptr);
  gtk_signal_connect(GTK_OBJECT(button4), "clicked",
                     GTK_SIGNAL_FUNC(dialogWinboloNetCloseBox), nullptr);
  gtk_signal_connect(GTK_OBJECT(button3), "clicked",
                     GTK_SIGNAL_FUNC(dialogWinbolonetOK), nullptr);

  dialogWbnUs = Winbolo_net_settings;
  return Winbolo_net_settings;
}
