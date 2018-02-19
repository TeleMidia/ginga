/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-glib.h"
#include "ginga_gtk.h"

GtkWidget *dialogWindow = NULL;

void
destroy_dialog_window ()
{
  gtk_widget_destroy (dialogWindow);
  dialogWindow = NULL;
  save_settings ();
}

void
accept_tracker_callback ()
{
  destroy_dialog_window ();
  presentationAttributes.showTrackerWindow = 0;
  trackerFlags.trackerAccept = 1;
  save_settings ();
  destroy_dialog_window ();
  send_http_log_message (0, (gchar *) "Open Ginga");
}

void
toggle_show_again ()
{
  if (presentationAttributes.showTrackerWindow == 0)
    presentationAttributes.showTrackerWindow = 1;
  else
    presentationAttributes.showTrackerWindow = 0;

  printf ("Show %d", presentationAttributes.showTrackerWindow);
}

void show_tracker_dialog (unused (GtkWindow *parent))
{
  if (dialogWindow != NULL || presentationAttributes.showTrackerWindow == 0)
    return;

  dialogWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (dialogWindow);
  gtk_window_set_title (GTK_WINDOW (dialogWindow), "Title");
  gtk_window_set_default_size (GTK_WINDOW (dialogWindow), 100, 100);
  gtk_window_set_position (GTK_WINDOW (dialogWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (dialogWindow), false);
  gtk_container_set_border_width (GTK_CONTAINER (dialogWindow), 5);

  GtkWidget *header_bar = gtk_header_bar_new ();
  g_assert_nonnull (header_bar);
  gtk_header_bar_set_title (GTK_HEADER_BAR (header_bar), "Ginga");
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header_bar), true);
  gtk_header_bar_set_decoration_layout (GTK_HEADER_BAR (header_bar),
                                        "menu:close");

  gtk_window_set_titlebar (GTK_WINDOW (dialogWindow), header_bar);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  g_assert_nonnull (box);

  GtkWidget *tools_gui_frame = gtk_frame_new (NULL);
  g_assert_nonnull (tools_gui_frame);
  gtk_frame_set_label_align (GTK_FRAME (tools_gui_frame), 0, 1.0);
  g_object_set (tools_gui_frame, "margin", 5, NULL);
  // g_object_set (tools_gui_frame, "padding", 5, NULL);

  GtkWidget *text = gtk_label_new (
      "\n To improve the development of Ginga Software. We would\n like to "
      "ask for your permission to share Ginga data with the\n developers "
      "team. If you agree, click the button bellow, if not,\n close this "
      "window.\n");

  gtk_container_add (GTK_CONTAINER (tools_gui_frame), text);

  GtkWidget *control_gui_frame = gtk_frame_new (NULL);
  g_assert_nonnull (control_gui_frame);

  GtkWidget *button_agree = gtk_button_new_with_label ("I Agree");
  g_signal_connect (button_agree, "clicked",
                    G_CALLBACK (accept_tracker_callback), NULL);

  gtk_container_add (GTK_CONTAINER (control_gui_frame), button_agree);

  GtkWidget *dont_show_again
      = gtk_check_button_new_with_label ("Don't show this message again");

  g_signal_connect (dont_show_again, "clicked",
                    G_CALLBACK (toggle_show_again), NULL);

  gtk_box_pack_start (GTK_BOX (box), tools_gui_frame, false, true, 0);
  gtk_box_pack_start (GTK_BOX (box), dont_show_again, false, true, 0);
  gtk_box_pack_start (GTK_BOX (box), control_gui_frame, false, true, 0);

  gtk_container_add (GTK_CONTAINER (dialogWindow), box);

  g_signal_connect (dialogWindow, "destroy",
                    G_CALLBACK (destroy_dialog_window), NULL);

  gtk_widget_show_all (dialogWindow);
}
