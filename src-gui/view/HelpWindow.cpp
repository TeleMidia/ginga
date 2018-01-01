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

#include "ginga_gtk.h"

GtkWidget *helpWindow = NULL;

void
create_help_window ()
{

  if (helpWindow != NULL)
    return;

  helpWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (helpWindow);
  gtk_window_set_title (GTK_WINDOW (helpWindow), "Ginga");
  gtk_window_set_default_size (GTK_WINDOW (helpWindow), 100, 100);
  gtk_window_set_position (GTK_WINDOW (helpWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (helpWindow), false);
  gtk_container_set_border_width (GTK_CONTAINER (helpWindow), 5);

  GtkWidget *header_bar = gtk_header_bar_new ();
  g_assert_nonnull (header_bar);
  gtk_header_bar_set_title (GTK_HEADER_BAR (header_bar), "Help");
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header_bar), true);
  gtk_header_bar_set_decoration_layout (GTK_HEADER_BAR (header_bar),
                                        "menu:close");

  gtk_window_set_titlebar (GTK_WINDOW (helpWindow), header_bar);

  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  g_assert_nonnull (box);

  GtkWidget *tools_gui_frame = gtk_frame_new ("Shortcuts");
  g_assert_nonnull (tools_gui_frame);
  gtk_frame_set_label_align (GTK_FRAME (tools_gui_frame), 0, 1.0);
  g_object_set (tools_gui_frame, "margin", 5, NULL);
  // g_object_set (tools_gui_frame, "padding", 5, NULL);

  GtkWidget *tool_grid = gtk_grid_new ();
  g_assert_nonnull (tool_grid);
  gtk_grid_set_row_spacing (GTK_GRID (tool_grid), 5);
  gtk_grid_set_row_homogeneous (GTK_GRID (tool_grid), false);
  gtk_grid_set_column_homogeneous (GTK_GRID (tool_grid), false);

  gtk_grid_attach (GTK_GRID (tool_grid), gtk_label_new (" "), 0, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (tool_grid), gtk_label_new ("Play (Ctrl+N)"), 1,
                   1, 1, 1);
  gtk_grid_attach (GTK_GRID (tool_grid), gtk_label_new ("Stop (Ctrl+S)"), 2,
                   1, 1, 1);
  gtk_grid_attach (GTK_GRID (tool_grid),
                   gtk_label_new (" FullScreen (Ctrl+F) "), 3, 1, 1, 1);

  gtk_grid_attach (GTK_GRID (tool_grid),
                   gtk_label_new (" R. Control (Ctrl+R) "), 1, 2, 1, 1);
  gtk_grid_attach (GTK_GRID (tool_grid),
                   gtk_label_new (" Console (Ctrl+D) "), 2, 2, 1, 1);

  gtk_grid_attach (GTK_GRID (tool_grid), gtk_label_new (" "), 4, 3, 1, 1);

  gtk_container_add (GTK_CONTAINER (tools_gui_frame), tool_grid);

  GtkWidget *control_gui_frame = gtk_frame_new ("Control Keys");
  g_assert_nonnull (control_gui_frame);
  g_object_set (control_gui_frame, "margin", 5, NULL);
  // g_object_set (control_gui_frame, "padding", 5, NULL);

  GtkWidget *control_grid = gtk_grid_new ();
  g_assert_nonnull (control_grid);
  gtk_grid_set_row_spacing (GTK_GRID (control_grid), 5);
  gtk_grid_set_row_homogeneous (GTK_GRID (control_grid), false);
  gtk_grid_set_column_homogeneous (GTK_GRID (control_grid), false);

  gtk_grid_attach (GTK_GRID (control_grid), gtk_label_new (" "), 0, 0, 1,
                   1);
  gtk_grid_attach (GTK_GRID (control_grid), gtk_label_new ("RED (F1) "), 1,
                   1, 1, 1);
  gtk_grid_attach (GTK_GRID (control_grid), gtk_label_new (" BLUE (F2) "),
                   2, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (control_grid), gtk_label_new (" YELLOW (F3) "),
                   3, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (control_grid), gtk_label_new (" BLUE (F4) "),
                   4, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (control_grid), gtk_label_new (" INFO (F5) "),
                   5, 1, 1, 1);
  gtk_grid_attach (GTK_GRID (control_grid), gtk_label_new (" "), 0, 2, 1,
                   1);

  gtk_container_add (GTK_CONTAINER (control_gui_frame), control_grid);

  gtk_box_pack_start (GTK_BOX (box), tools_gui_frame, false, true, 0);
  gtk_box_pack_start (GTK_BOX (box), control_gui_frame, false, true, 0);

  gtk_container_add (GTK_CONTAINER (helpWindow), box);

  g_signal_connect (helpWindow, "destroy", G_CALLBACK (destroy_help_window),
                    NULL);

  gtk_widget_show_all (helpWindow);
}

void
destroy_help_window ()
{
  gtk_widget_destroy (helpWindow);
  helpWindow = NULL;
}
