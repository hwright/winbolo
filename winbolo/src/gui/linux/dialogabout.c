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
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "../lang.h"
#include "../linresource.h"
#include "boloicon.xpm"

GtkWidget *dialogAboutUs;

gboolean dialogAboutFunc(GtkWidget *widget,  GdkEventButton *event, gpointer user_data) {
  gtk_grab_remove(dialogAboutUs);
  gtk_widget_destroy(dialogAboutUs);
  gtk_main_quit();
  return false;
}

GtkWidget* dialogAboutCreate(void) {
  GtkWidget *dialogAbout;
  GtkWidget *vbox1;
  GtkWidget *hbox1;
  GtkWidget *pixmap1;
  GtkWidget *label2;
  GtkWidget *label1;
  GtkWidget *button1;
  GdkBitmap *pixmap_mask;
  GdkPixmap *pixmap_data;
    
  dialogAbout = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (dialogAbout), "dialogAbout", dialogAbout);
  gtk_window_set_position (GTK_WINDOW (dialogAbout), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (dialogAbout), true);
  gtk_window_set_policy (GTK_WINDOW (dialogAbout), false, false, false);
  gtk_container_set_border_width (GTK_CONTAINER (dialogAbout), 10);
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAbout), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (dialogAbout), vbox1);

  hbox1 = gtk_hbox_new (false, 0);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAbout), "hbox1", hbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, true, true, 0);

  pixmap_data = gdk_pixmap_colormap_create_from_xpm_d(NULL, gtk_widget_get_colormap(dialogAbout), &pixmap_mask, NULL, boloicon_xpm);
  pixmap1 = gtk_pixmap_new(pixmap_data, pixmap_mask);
  gtk_widget_ref (pixmap1);
  gdk_pixmap_unref(pixmap_data);
  gdk_pixmap_unref(pixmap_mask);

  gtk_widget_ref (pixmap1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAbout), "pixmap1", pixmap1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap1);
  gtk_box_pack_start (GTK_BOX (hbox1), pixmap1, true, true, 0);

  label2 = gtk_label_new ("LinBolo  - v1.13\nLinBolo Copyright 1998-2003 John Morrison\nBolo Copyright 1987-1995 Stuart Cheshire\n");
  gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (dialogAbout), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox1), label2, false, false, 0);

  label1 = gtk_label_new (langGetText(STR_DLGABOUT_BLURB));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAbout), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (vbox1), label1, false, false, 0);

  button1 = gtk_button_new_with_label ("OK");
  gtk_widget_ref (button1);
  gtk_object_set_data_full (GTK_OBJECT (dialogAbout), "button1", button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (button1);
  gtk_box_pack_start (GTK_BOX (vbox1), button1, false, false, 0);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  gtk_widget_grab_focus (button1);
  gtk_widget_grab_default (button1);
  gtk_signal_connect(GTK_OBJECT(dialogAbout), "delete_event", GTK_SIGNAL_FUNC (dialogAboutFunc), NULL);
  gtk_signal_connect(GTK_OBJECT(button1), "clicked", GTK_SIGNAL_FUNC(dialogAboutFunc), 0);
    
  dialogAboutUs = dialogAbout;
  return dialogAbout;
}

