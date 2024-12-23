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

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../bolo/backend.h"
#include "input.h"

static keyItems keySetupKeys; /* Copy of the keys for use by the dialog box */
static GtkWidget *dialogKeySetupUs;
static GtkWidget *idc_keyautoshow;
static GtkWidget *idc_keyautoslow;

void windowSetKeys(keyItems *value);

static gboolean on_press(GtkWidget *widget, GdkEventKey *event,
                         gpointer user_data) {
  uint *ptr;
  ptr = (uint *)(user_data);

  *ptr = gdk_keyval_to_upper(event->keyval);
  gtk_entry_set_text(GTK_ENTRY(widget),
                     gdk_keyval_name(gdk_keyval_to_upper(event->keyval)));
  gtk_signal_emit_stop_by_name(GTK_OBJECT(widget), "key_press_event");
  return FALSE;
}

static void on_button2_clicked(GtkButton *button, gpointer user_data) {
  gtk_grab_remove(dialogKeySetupUs);
  gtk_widget_destroy(dialogKeySetupUs);
  gtk_main_quit();
}

static void on_button1_clicked(GtkButton *button, gpointer user_data) {
  /* Autoslowdown */
  screenSetTankAutoSlowdown(
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_keyautoslow)));
  screenSetTankAutoHideGunsight(
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(idc_keyautoshow)));
  windowSetKeys(&keySetupKeys);
  gtk_grab_remove(dialogKeySetupUs);
  gtk_widget_destroy(dialogKeySetupUs);
  gtk_main_quit();
}

/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */
GtkWidget *dialogKeySetupCreate(keyItems *value) {
  GtkWidget *dialogKeySetup;
  GtkWidget *vbox1;
  GtkWidget *label1;
  GtkWidget *table1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label5;
  GtkWidget *label6;
  GtkWidget *label7;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *label12;
  GtkWidget *label13;
  GtkWidget *label14;
  GtkWidget *label15;
  GtkWidget *label16;
  GtkWidget *label17;
  GtkWidget *label18;
  GtkWidget *label19;
  GtkWidget *label20;
  GtkWidget *label21;
  GtkWidget *idc_keyforward;
  GtkWidget *idc_keybackward;
  GtkWidget *idc_keyturnleft;
  GtkWidget *idc_keyturnright;
  GtkWidget *idc_keyincrease;
  GtkWidget *idc_keydecrease;
  GtkWidget *idc_keyshoot;
  GtkWidget *idc_keymine;
  GtkWidget *idc_keypillview;
  GtkWidget *idc_keytankview;
  GtkWidget *idc_keyup;
  GtkWidget *idc_keydown;
  GtkWidget *idc_keyleft;
  GtkWidget *label22;
  GtkWidget *label23;
  GtkWidget *idc_keyright;
  GtkWidget *label24;
  GtkWidget *label25;
  GtkWidget *label26;
  GtkWidget *button2;
  GtkWidget *button1;

  keySetupKeys.kiForward = value->kiForward;
  keySetupKeys.kiBackward = value->kiBackward;
  keySetupKeys.kiLeft = value->kiLeft;
  keySetupKeys.kiRight = value->kiRight;
  keySetupKeys.kiShoot = value->kiShoot;
  keySetupKeys.kiLayMine = value->kiLayMine;
  keySetupKeys.kiGunIncrease = value->kiGunIncrease;
  keySetupKeys.kiGunDecrease = value->kiGunDecrease;
  keySetupKeys.kiTankView = value->kiTankView;
  keySetupKeys.kiPillView = value->kiPillView;
  keySetupKeys.kiScrollUp = value->kiScrollUp;
  keySetupKeys.kiScrollDown = value->kiScrollDown;
  keySetupKeys.kiScrollLeft = value->kiScrollLeft;
  keySetupKeys.kiScrollRight = value->kiScrollRight;

  dialogKeySetup = gtk_dialog_new();
  gtk_object_set_data(GTK_OBJECT(dialogKeySetup), "dialogKeySetup",
                      dialogKeySetup);
  gtk_container_set_border_width(GTK_CONTAINER(dialogKeySetup), 15);
  gtk_window_set_title(GTK_WINDOW(dialogKeySetup), "Choose Key Settings");
  gtk_window_set_position(GTK_WINDOW(dialogKeySetup), GTK_WIN_POS_CENTER);
  gtk_window_set_modal(GTK_WINDOW(dialogKeySetup), TRUE);
  gtk_window_set_policy(GTK_WINDOW(dialogKeySetup), FALSE, FALSE, FALSE);

  vbox1 = gtk_dialog_get_content_area(GTK_DIALOG(dialogKeySetup));

  label1 = gtk_label_new(
      "Click on the option you wish to change then the next key pressed will "
      "be assigned to it.");
  gtk_widget_ref(label1);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label1", label1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label1);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);
  gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label1), 7.45058e-09, 0.5);

  table1 = gtk_table_new(17, 4, FALSE);
  gtk_widget_ref(table1);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "table1", table1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(table1);
  gtk_box_pack_start(GTK_BOX(vbox1), table1, TRUE, TRUE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(table1), 6);

  label2 = gtk_label_new("Drive tank");
  gtk_widget_ref(label2);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label2", label2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label2);
  gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 0, 1,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_LEFT);
  gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

  label3 = gtk_label_new("Rotate Tank:");
  gtk_widget_ref(label3);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label3", label3,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label3);
  gtk_table_attach(GTK_TABLE(table1), label3, 0, 1, 2, 3,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label3), 7.45058e-09, 0.5);

  label4 = gtk_label_new("Gun Range:");
  gtk_widget_ref(label4);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label4", label4,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label4);
  gtk_table_attach(GTK_TABLE(table1), label4, 0, 1, 5, 6,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label4), 0, 0.5);

  label5 = gtk_label_new("Weapons:");
  gtk_widget_ref(label5);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label5", label5,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label5);
  gtk_table_attach(GTK_TABLE(table1), label5, 0, 1, 8, 9,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label5), 0, 0.5);

  label6 = gtk_label_new("Change Views: ");
  gtk_widget_ref(label6);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label6", label6,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label6);
  gtk_table_attach(GTK_TABLE(table1), label6, 0, 1, 10, 11,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label6), 0, 0.5);

  label7 = gtk_label_new("Scroll:");
  gtk_widget_ref(label7);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label7", label7,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label7);
  gtk_table_attach(GTK_TABLE(table1), label7, 0, 1, 13, 14,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label7), 0, 0.5);

  label8 = gtk_label_new("Accelerate");
  gtk_widget_ref(label8);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label8", label8,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label8);
  gtk_table_attach(GTK_TABLE(table1), label8, 1, 2, 0, 1,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label8), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label8), 10, 0);

  label9 = gtk_label_new("Decelerate");
  gtk_widget_ref(label9);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label9", label9,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label9);
  gtk_table_attach(GTK_TABLE(table1), label9, 1, 2, 1, 2,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label9), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label9), 10, 0);

  label10 = gtk_label_new("Anti-clockwise");
  gtk_widget_ref(label10);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label10", label10,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label10);
  gtk_table_attach(GTK_TABLE(table1), label10, 1, 2, 2, 3,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label10), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label10), 10, 0);

  label11 = gtk_label_new("Clockwise");
  gtk_widget_ref(label11);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label11", label11,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label11);
  gtk_table_attach(GTK_TABLE(table1), label11, 1, 2, 3, 4,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label11), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label11), 10, 0);

  label12 = gtk_label_new("Increase");
  gtk_widget_ref(label12);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label12", label12,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label12);
  gtk_table_attach(GTK_TABLE(table1), label12, 1, 2, 5, 6,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label12), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label12), 10, 0);

  label13 = gtk_label_new("Decrease");
  gtk_widget_ref(label13);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label13", label13,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label13);
  gtk_table_attach(GTK_TABLE(table1), label13, 1, 2, 6, 7,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label13), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label13), 10, 0);

  label14 = gtk_label_new("Shoot");
  gtk_widget_ref(label14);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label14", label14,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label14);
  gtk_table_attach(GTK_TABLE(table1), label14, 1, 2, 8, 9,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label14), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label14), 10, 0);

  label15 = gtk_label_new("Lay mine");
  gtk_widget_ref(label15);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label15", label15,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label15);
  gtk_table_attach(GTK_TABLE(table1), label15, 1, 2, 9, 10,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label15), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label15), 10, 0);

  label16 = gtk_label_new("Tank view");
  gtk_widget_ref(label16);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label16", label16,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label16);
  gtk_table_attach(GTK_TABLE(table1), label16, 1, 2, 10, 11,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label16), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label16), 10, 0);

  label17 = gtk_label_new("Pill view");
  gtk_widget_ref(label17);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label17", label17,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label17);
  gtk_table_attach(GTK_TABLE(table1), label17, 1, 2, 11, 12,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label17), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label17), 10, 0);

  label18 = gtk_label_new("Up");
  gtk_widget_ref(label18);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label18", label18,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label18);
  gtk_table_attach(GTK_TABLE(table1), label18, 1, 2, 13, 14,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label18), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label18), 10, 0);

  label19 = gtk_label_new("Down");
  gtk_widget_ref(label19);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label19", label19,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label19);
  gtk_table_attach(GTK_TABLE(table1), label19, 1, 2, 14, 15,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label19), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label19), 10, 0);

  label20 = gtk_label_new("Left");
  gtk_widget_ref(label20);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label20", label20,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label20);
  gtk_table_attach(GTK_TABLE(table1), label20, 1, 2, 15, 16,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label20), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label20), 10, 0);

  label21 = gtk_label_new("Right");
  gtk_widget_ref(label21);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label21", label21,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label21);
  gtk_table_attach(GTK_TABLE(table1), label21, 1, 2, 16, 17,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label21), 1, 0.5);
  gtk_misc_set_padding(GTK_MISC(label21), 10, 0);

  idc_keyforward = gtk_entry_new();
  gtk_widget_ref(idc_keyforward);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyforward",
                           idc_keyforward, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyforward);
  gtk_table_attach(GTK_TABLE(table1), idc_keyforward, 2, 3, 0, 1,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keybackward = gtk_entry_new();
  gtk_widget_ref(idc_keybackward);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keybackward",
                           idc_keybackward, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keybackward);
  gtk_table_attach(GTK_TABLE(table1), idc_keybackward, 2, 3, 1, 2,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keyturnleft = gtk_entry_new();
  gtk_widget_ref(idc_keyturnleft);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyturnleft",
                           idc_keyturnleft, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyturnleft);
  gtk_table_attach(GTK_TABLE(table1), idc_keyturnleft, 2, 3, 2, 3,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keyturnright = gtk_entry_new();
  gtk_widget_ref(idc_keyturnright);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyturnright",
                           idc_keyturnright,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyturnright);
  gtk_table_attach(GTK_TABLE(table1), idc_keyturnright, 2, 3, 3, 4,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keyincrease = gtk_entry_new();
  gtk_widget_ref(idc_keyincrease);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyincrease",
                           idc_keyincrease, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyincrease);
  gtk_table_attach(GTK_TABLE(table1), idc_keyincrease, 2, 3, 5, 6,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keydecrease = gtk_entry_new();
  gtk_widget_ref(idc_keydecrease);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keydecrease",
                           idc_keydecrease, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keydecrease);
  gtk_table_attach(GTK_TABLE(table1), idc_keydecrease, 2, 3, 6, 7,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keyshoot = gtk_entry_new();
  gtk_widget_ref(idc_keyshoot);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyshoot",
                           idc_keyshoot, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyshoot);
  gtk_table_attach(GTK_TABLE(table1), idc_keyshoot, 2, 3, 8, 9,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keymine = gtk_entry_new();
  gtk_widget_ref(idc_keymine);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keymine",
                           idc_keymine, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keymine);
  gtk_table_attach(GTK_TABLE(table1), idc_keymine, 2, 3, 9, 10,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keypillview = gtk_entry_new();
  gtk_widget_ref(idc_keypillview);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keypillview",
                           idc_keypillview, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keypillview);
  gtk_table_attach(GTK_TABLE(table1), idc_keypillview, 2, 3, 11, 12,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keytankview = gtk_entry_new();
  gtk_widget_ref(idc_keytankview);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keytankview",
                           idc_keytankview, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keytankview);
  gtk_table_attach(GTK_TABLE(table1), idc_keytankview, 2, 3, 10, 11,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keyup = gtk_entry_new();
  gtk_widget_ref(idc_keyup);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyup", idc_keyup,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyup);
  gtk_table_attach(GTK_TABLE(table1), idc_keyup, 2, 3, 13, 14,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keydown = gtk_entry_new();
  gtk_widget_ref(idc_keydown);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keydown",
                           idc_keydown, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keydown);
  gtk_table_attach(GTK_TABLE(table1), idc_keydown, 2, 3, 14, 15,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keyleft = gtk_entry_new();
  gtk_widget_ref(idc_keyleft);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyleft",
                           idc_keyleft, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyleft);
  gtk_table_attach(GTK_TABLE(table1), idc_keyleft, 2, 3, 15, 16,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  label22 = gtk_label_new("(left)");
  gtk_widget_ref(label22);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label22", label22,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label22);
  gtk_table_attach(GTK_TABLE(table1), label22, 3, 4, 2, 3,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label22), 7.45058e-09, 0.5);
  gtk_misc_set_padding(GTK_MISC(label22), 10, 0);

  label23 = gtk_label_new("(right)");
  gtk_widget_ref(label23);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label23", label23,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label23);
  gtk_table_attach(GTK_TABLE(table1), label23, 3, 4, 3, 4,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 0, 0);
  gtk_misc_set_alignment(GTK_MISC(label23), 7.45058e-09, 0.5);
  gtk_misc_set_padding(GTK_MISC(label23), 10, 0);

  idc_keyright = gtk_entry_new();
  gtk_widget_ref(idc_keyright);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyright",
                           idc_keyright, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyright);
  gtk_table_attach(GTK_TABLE(table1), idc_keyright, 2, 3, 16, 17,
                   (GtkAttachOptions)(GTK_EXPAND | GTK_FILL),
                   (GtkAttachOptions)(0), 0, 0);

  idc_keyautoshow = gtk_check_button_new_with_label(
      "Enable Automatic\nshow and hide of\nthe gunsight");
  gtk_widget_ref(idc_keyautoshow);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyautoshow",
                           idc_keyautoshow, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyautoshow);
  gtk_table_attach(GTK_TABLE(table1), idc_keyautoshow, 3, 4, 5, 6,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 5, 0);

  idc_keyautoslow = gtk_check_button_new_with_label("Auto slowdown");
  gtk_widget_ref(idc_keyautoslow);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "idc_keyautoslow",
                           idc_keyautoslow, (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(idc_keyautoslow);
  gtk_table_attach(GTK_TABLE(table1), idc_keyautoslow, 3, 4, 0, 1,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 5, 0);

  label24 = gtk_label_new("");
  gtk_widget_ref(label24);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label24", label24,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label24);
  gtk_table_attach(GTK_TABLE(table1), label24, 2, 3, 12, 13,
                   (GtkAttachOptions)(0), (GtkAttachOptions)(0), 0, 0);

  label25 = gtk_label_new("");
  gtk_widget_ref(label25);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label25", label25,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label25);
  gtk_table_attach(GTK_TABLE(table1), label25, 2, 3, 7, 8,
                   (GtkAttachOptions)(0), (GtkAttachOptions)(0), 0, 0);

  label26 = gtk_label_new("");
  gtk_widget_ref(label26);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "label26", label26,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(label26);
  gtk_table_attach(GTK_TABLE(table1), label26, 2, 3, 4, 5,
                   (GtkAttachOptions)(0), (GtkAttachOptions)(0), 0, 0);

  button2 = gtk_button_new_with_label("Cancel");
  gtk_widget_ref(button2);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "button2", button2,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button2);
  gtk_table_attach(GTK_TABLE(table1), button2, 3, 4, 16, 17,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 10, 0);

  button1 = gtk_button_new_with_label("OK");
  gtk_widget_ref(button1);
  gtk_object_set_data_full(GTK_OBJECT(dialogKeySetup), "button1", button1,
                           (GtkDestroyNotify)gtk_widget_unref);
  gtk_widget_show(button1);
  gtk_table_attach(GTK_TABLE(table1), button1, 3, 4, 15, 16,
                   (GtkAttachOptions)(GTK_FILL), (GtkAttachOptions)(0), 10, 0);
  GTK_WIDGET_SET_FLAGS(button1, GTK_CAN_DEFAULT);

  gtk_entry_set_text(
      GTK_ENTRY(idc_keyforward),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiForward)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keybackward),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiBackward)));
  gtk_entry_set_text(GTK_ENTRY(idc_keyturnleft),
                     gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiLeft)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keyturnright),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiRight)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keyshoot),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiShoot)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keymine),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiLayMine)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keytankview),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiTankView)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keypillview),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiPillView)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keyup),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiScrollUp)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keydown),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiScrollDown)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keyleft),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiScrollLeft)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keyright),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiScrollRight)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keyincrease),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiGunIncrease)));
  gtk_entry_set_text(
      GTK_ENTRY(idc_keydecrease),
      gdk_keyval_name(gdk_keyval_to_upper(keySetupKeys.kiGunDecrease)));

  gtk_signal_connect(GTK_OBJECT(idc_keyforward), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiForward));
  gtk_signal_connect(GTK_OBJECT(idc_keybackward), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiBackward));
  gtk_signal_connect(GTK_OBJECT(idc_keyturnleft), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiLeft));
  gtk_signal_connect(GTK_OBJECT(idc_keyturnright), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiRight));
  gtk_signal_connect(GTK_OBJECT(idc_keyincrease), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiGunIncrease));
  gtk_signal_connect(GTK_OBJECT(idc_keydecrease), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiGunDecrease));
  gtk_signal_connect(GTK_OBJECT(idc_keyshoot), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiShoot));
  gtk_signal_connect(GTK_OBJECT(idc_keymine), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiLayMine));
  gtk_signal_connect(GTK_OBJECT(idc_keypillview), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiPillView));
  gtk_signal_connect(GTK_OBJECT(idc_keytankview), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiTankView));
  gtk_signal_connect(GTK_OBJECT(idc_keyup), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiScrollUp));
  gtk_signal_connect(GTK_OBJECT(idc_keydown), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiScrollDown));
  gtk_signal_connect(GTK_OBJECT(idc_keyleft), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiScrollLeft));
  gtk_signal_connect(GTK_OBJECT(idc_keyright), "key_press_event",
                     GTK_SIGNAL_FUNC(on_press), &(keySetupKeys.kiScrollRight));
  gtk_signal_connect(GTK_OBJECT(button2), "clicked",
                     GTK_SIGNAL_FUNC(on_button2_clicked), nullptr);
  gtk_signal_connect(GTK_OBJECT(button1), "clicked",
                     GTK_SIGNAL_FUNC(on_button1_clicked), nullptr);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(idc_keyautoslow),
                               screenGetTankAutoSlowdown());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(idc_keyautoshow),
                               screenGetTankAutoHideGunsight());
  gtk_widget_grab_focus(button1);
  gtk_widget_grab_default(button1);
  dialogKeySetupUs = dialogKeySetup;
  return dialogKeySetup;
}
