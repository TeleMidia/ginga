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
create_key_combobox (GtkWidget *combo_box)
{
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F1");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F2");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F3");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F4");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F5");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F6");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F7");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F8");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "F9");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1,
                                  "F10");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1,
                                  "F11");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1,
                                  "F12");
}

void
create_settings_window (void)
{
  settingsWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (settingsWindow);
  gtk_window_set_title (GTK_WINDOW (settingsWindow), "Settings");
  gtk_window_set_default_size (GTK_WINDOW (settingsWindow), 200, 200);
  gtk_window_set_position (GTK_WINDOW (settingsWindow), GTK_WIN_POS_CENTER);
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
  /*
    GtkWidget *red_icon = gtk_image_new_from_file (
        g_strconcat (ginga_gui.executable_folder,
                     "icons/light-theme/settings-icon.png", NULL));
    gtk_fixed_put (GTK_FIXED (fixed_layout_controls), button_icon, 0, 0);

    GtkWidget *combo_box = gtk_combo_box_text_new ();
    g_assert_nonnull (combo_box);
    create_key_combobox (combo_box);
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);
    gtk_fixed_put (GTK_FIXED (fixed_layout_controls), combo_box, 50, 0);
   */
  GtkWidget *fixed_layout_presentation = gtk_fixed_new ();
  g_assert_nonnull (fixed_layout_presentation);

  GtkWidget *combo_box = gtk_combo_box_text_new ();
  g_assert_nonnull (combo_box);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "4:3");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "16:9");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1, "16:10");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box), 0);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), combo_box, 100, 10);
  
  GtkWidget *aspect_label = gtk_label_new ("Aspect ratio");
  gtk_label_set_markup (
      GTK_LABEL (aspect_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "Aspect ratio"));
  g_assert_nonnull (aspect_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), aspect_label,
                 10,(BUTTON_SIZE/3)+25);

  GtkWidget *width_label = gtk_label_new ("Width");
  gtk_label_set_markup (
      GTK_LABEL (width_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "Width"));
  g_assert_nonnull (width_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), width_label,
                 10, (BUTTON_SIZE/3)+ BUTTON_SIZE + 20) ;

  GtkWidget *height_label = gtk_label_new ("Height");
  gtk_label_set_markup (
      GTK_LABEL (height_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "Height"));
  g_assert_nonnull (height_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), height_label,
                 10, (BUTTON_SIZE/3)+ (BUTTON_SIZE*2) + 15) ;

  GtkWidget *width_entry = gtk_entry_new ();
  g_assert_nonnull (width_entry);
  gtk_widget_set_size_request (width_entry,
                               20,
                               BUTTON_SIZE);
  gtk_entry_set_text (GTK_ENTRY (width_entry),
                      "800");
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), width_entry,
                 100, (BUTTON_SIZE) + 20);

   GtkWidget *height_entry = gtk_entry_new ();
  g_assert_nonnull (height_entry);
  gtk_widget_set_size_request (height_entry,
                               20,
                               BUTTON_SIZE);
  gtk_entry_set_text (GTK_ENTRY (height_entry),
                      "600");
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), height_entry,
                 100, (BUTTON_SIZE*2) + 25);             

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                            fixed_layout_presentation,
                            gtk_label_new ("Presentation"));
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), fixed_layout_controls,
                            gtk_label_new ("Controls"));


  gtk_fixed_put (GTK_FIXED (fixed_layout), notebook, 0, 0);
  gtk_container_add (GTK_CONTAINER (settingsWindow), fixed_layout);
  gtk_widget_show_all (settingsWindow);
}

void
destroy_settings_window (void)
{
}