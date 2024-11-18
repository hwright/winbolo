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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "../../bolo/global.h"
#include "../../bolo/network.h"
#include "../linresource.h"
#include "../lang.h"

static GtkWidget *label1;
static GtkWidget *dialogAllianceUs;
static BYTE dialogAlliancePlayerNum;
static bool dialogAllianceIsShowing = FALSE;

void dialogAllianceSetName(char *playerName, BYTE playerNum) {
  char output[FILENAME_MAX];
  
  dialogAlliancePlayerNum = playerNum;
  strcpy(output, playerName);
  strcat(output, langGetText(STR_DLGALLIANCE_BLURB));

  gtk_label_set_text(GTK_LABEL(label1), output);
  gdk_threads_enter();
  gtk_widget_show(dialogAllianceUs);
  gdk_threads_leave();
  dialogAllianceIsShowing = TRUE;
}

static gboolean dialogAllianceAccept(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
  netAllianceAccept(dialogAlliancePlayerNum);
  dialogAllianceIsShowing = FALSE;
  gtk_widget_hide(dialogAllianceUs);
  return FALSE;
}
static gboolean dialogAllianceReject(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
  dialogAllianceIsShowing = FALSE;
  gtk_widget_hide(dialogAllianceUs);
    
  return FALSE;
}

GtkWidget* dialogAllianceCreate (void) {
  GtkWidget *dialogAlliance;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *button1;
  GtkWidget *button2;

  dialogAlliance = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (dialogAlliance), "dialogAlliance", dialogAlliance);
  gtk_container_set_border_width (GTK_CONTAINER (dialogAlliance), 15);
  gtk_window_set_title (GTK_WINDOW (dialogAlliance), "Alliance Request");

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAlliance), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (dialogAlliance), vbox1);

  label1 = gtk_label_new ("%s requests alliance. Accept?\n");
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAlliance), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, FALSE, FALSE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAlliance), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  button1 = gtk_button_new_with_label ("Accept");
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAlliance), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_box_pack_start (GTK_BOX (hbox1), button1, TRUE, TRUE, 0);
  GTK_WIDGET_UNSET_FLAGS (button1, GTK_CAN_FOCUS);

  button2 = gtk_button_new_with_label ("Reject");
  gtk_widget_ref (button2);
  gtk_object_set_data_full (GTK_OBJECT (dialogAlliance), "button2", button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button2);
  gtk_box_pack_start (GTK_BOX (hbox1), button2, TRUE, TRUE, 0);

  gtk_signal_connect(GTK_OBJECT(button1), "clicked", GTK_SIGNAL_FUNC(dialogAllianceAccept), nullptr);
  gtk_signal_connect(GTK_OBJECT(button2), "clicked", GTK_SIGNAL_FUNC(dialogAllianceReject), nullptr);

  dialogAllianceUs = dialogAlliance;
  return dialogAlliance;
}
