/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "ginga_gtk.h"

GtkWidget *settingsWindow = NULL;

void
create_settings_window (void)
{
  settingsWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (settingsWindow);
  gtk_window_set_title (GTK_WINDOW (settingsWindow), "Settings");
  gtk_window_set_default_size (GTK_WINDOW (settingsWindow), 200,
                               200);
  gtk_window_set_position (GTK_WINDOW (settingsWindow),
                           GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (settingsWindow), FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (settingsWindow),
                                  ginga_gui.default_margin);

  GtkWidget *fixed_layout = gtk_fixed_new ();
  g_assert_nonnull (ginga_gui.fixed_layout);

  GtkWidget *notebook = gtk_notebook_new ();
  g_assert_nonnull (notebook);

  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook), TRUE);

  GtkWidget *fixed_layout_controls = gtk_fixed_new ();
  g_assert_nonnull (fixed_layout_controls);

  GtkWidget *c1_label = gtk_label_new ("Red");
  g_assert_nonnull (c1_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_controls), c1_label, 0, 0);

  GtkWidget *button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  GtkWidget *button_vol_up = gtk_button_new ();
  g_assert_nonnull (button_vol_up);
  gtk_button_set_image (GTK_BUTTON (button_vol_up), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout_controls), button_vol_up, 0, 0);

  GtkWidget *combo_box = gtk_combo_box_text_new ();
  g_assert_nonnull (combo_box);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F1");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);
  gtk_fixed_put (GTK_FIXED (fixed_layout_controls), combo_box, 50, 0);

  GtkWidget *fixed_layout_presentation = gtk_fixed_new ();
  g_assert_nonnull (fixed_layout_presentation);

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), fixed_layout_controls,
                            gtk_label_new ("Controls"));
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                            fixed_layout_presentation,
                            gtk_label_new ("Presentation"));

  gtk_fixed_put (GTK_FIXED (fixed_layout), notebook, 0, 0);
  gtk_container_add (GTK_CONTAINER (settingsWindow),
                     fixed_layout);
  gtk_widget_show_all (settingsWindow);
}

void
destroy_settings_window (void)
{
}