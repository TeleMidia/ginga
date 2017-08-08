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
PresentationAttributes presentationAttributes;
PresentationAttributes original_presentationAttributes;
GtkWidget *widthEntry = NULL;
GtkWidget *heightEntry = NULL;

void
apply_settings_callback (void)
{
  original_presentationAttributes = presentationAttributes;
  resize_main_window_canvas ();
}

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
fps_combobox_changed (GtkComboBox *widget, gpointer user_data)
{
  presentationAttributes.frameRate
      = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));
}

void
aspect_combobox_changed (GtkComboBox *widget, gpointer user_data)
{
  presentationAttributes.aspectRatio
      = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));

  if (presentationAttributes.resolutionWidth
      >= presentationAttributes.resolutionHeight)
    {
      if (presentationAttributes.aspectRatio == 0)
        presentationAttributes.resolutionHeight
            = (presentationAttributes.resolutionWidth * 3) / 4;
      else if (presentationAttributes.aspectRatio == 1)
        presentationAttributes.resolutionHeight
            = (presentationAttributes.resolutionWidth * 9) / 16;
      else if (presentationAttributes.aspectRatio == 2)
        presentationAttributes.resolutionHeight
            = (presentationAttributes.resolutionWidth * 10) / 16;
      gtk_entry_set_text (
          GTK_ENTRY (heightEntry),
          g_markup_printf_escaped (
              "%d", presentationAttributes.resolutionHeight));
    }
  else
    {
      if (presentationAttributes.aspectRatio == 0)
        presentationAttributes.resolutionWidth
            = (presentationAttributes.resolutionHeight * 4) / 3;
      else if (presentationAttributes.aspectRatio == 1)
        presentationAttributes.resolutionWidth
            = (presentationAttributes.resolutionHeight * 16) / 9;
      else if (presentationAttributes.aspectRatio == 2)
        presentationAttributes.resolutionWidth
            = (presentationAttributes.resolutionHeight * 16) / 10;
      gtk_entry_set_text (
          GTK_ENTRY (widthEntry),
          g_markup_printf_escaped ("%d",
                                   presentationAttributes.resolutionWidth));
    }
}

void
preedit_changed_callback (GtkEditable *edit, gchar *new_text,
                          gint new_length, gpointer position, gpointer data)
{
  const gchar *entry_str = gtk_entry_get_text (GTK_ENTRY (edit));
  gchar *content = g_strconcat (entry_str, new_text, NULL);

  if (strlen (content) == 0)
    return;

  for (guint8 i = 0; i < strlen (content); i++)
    {
      if (content[i] < '0' || content[i] > '9')
        {
          content[i] = 0;
          break;
        }
    }

  if (widthEntry == (GtkWidget *)edit)
    {
      gint64 value = atoi (content);
      if (presentationAttributes.resolutionWidth != value)
        presentationAttributes.resolutionWidth = value;
      else
        return;
      presentationAttributes.resolutionWidth = atoi (content);
      if (presentationAttributes.aspectRatio == 0)
        presentationAttributes.resolutionHeight
            = (presentationAttributes.resolutionWidth * 3) / 4;
      else if (presentationAttributes.aspectRatio == 1)
        presentationAttributes.resolutionHeight
            = (presentationAttributes.resolutionWidth * 9) / 16;
      else if (presentationAttributes.aspectRatio == 2)
        presentationAttributes.resolutionHeight
            = (presentationAttributes.resolutionWidth * 10) / 16;
      gtk_entry_set_text (
          GTK_ENTRY (heightEntry),
          g_markup_printf_escaped (
              "%d", presentationAttributes.resolutionHeight));
    }
  else
    {
      gint64 value = atoi (content);
      if (presentationAttributes.resolutionHeight != value)
        presentationAttributes.resolutionHeight = value;
      else
        return;
      if (presentationAttributes.aspectRatio == 0)
        presentationAttributes.resolutionWidth
            = (presentationAttributes.resolutionHeight * 4) / 3;
      else if (presentationAttributes.aspectRatio == 1)
        presentationAttributes.resolutionWidth
            = (presentationAttributes.resolutionHeight * 16) / 9;
      else if (presentationAttributes.aspectRatio == 2)
        presentationAttributes.resolutionWidth
            = (presentationAttributes.resolutionHeight * 16) / 10;
      gtk_entry_set_text (
          GTK_ENTRY (widthEntry),
          g_markup_printf_escaped ("%d",
                                   presentationAttributes.resolutionWidth));
    }
}

void
create_presentation_page (GtkWidget *notebook)
{
  GtkWidget *fixed_layout_presentation = gtk_fixed_new ();
  g_assert_nonnull (fixed_layout_presentation);

  GtkWidget *combo_box = gtk_combo_box_text_new ();
  g_assert_nonnull (combo_box);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1,
                                  "TV (4:3)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1,
                                  "HDTV (16:9)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (combo_box), -1,
                                  "LCD (16:10)");
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box),
                            presentationAttributes.aspectRatio);
  g_signal_connect (combo_box, "changed",
                    G_CALLBACK (aspect_combobox_changed), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), combo_box, 120, 10);

  GtkWidget *aspect_label = gtk_label_new ("Aspect ratio");
  gtk_label_set_markup (
      GTK_LABEL (aspect_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "Aspect ratio"));
  g_assert_nonnull (aspect_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), aspect_label, 30,
                 (BUTTON_SIZE / 3) + 10);

  GtkWidget *width_label = gtk_label_new ("Width");
  gtk_label_set_markup (
      GTK_LABEL (width_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "Width"));
  g_assert_nonnull (width_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), width_label, 30,
                 (BUTTON_SIZE / 3) + BUTTON_SIZE + 20);

  GtkWidget *height_label = gtk_label_new ("Height");
  gtk_label_set_markup (
      GTK_LABEL (height_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "Height"));
  g_assert_nonnull (height_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), height_label, 125,
                 (BUTTON_SIZE / 3) + BUTTON_SIZE + 20);

  GtkWidget *inv_label = gtk_label_new (" ");
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), inv_label, 250,
                 140);

  widthEntry = gtk_entry_new ();
  g_assert_nonnull (widthEntry);
  gtk_entry_set_width_chars (GTK_ENTRY (widthEntry), 4);
  g_signal_connect (widthEntry, "insert-text",
                    G_CALLBACK (preedit_changed_callback), NULL);
  gtk_entry_set_text (GTK_ENTRY (widthEntry),
                      g_markup_printf_escaped (
                          "%d", presentationAttributes.resolutionWidth));
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), widthEntry, 70,
                 (BUTTON_SIZE) + 20);

  heightEntry = gtk_entry_new ();
  g_assert_nonnull (heightEntry);
  gtk_entry_set_width_chars (GTK_ENTRY (heightEntry), 4);
  g_signal_connect (heightEntry, "insert-text",
                    G_CALLBACK (preedit_changed_callback), NULL);
  gtk_entry_set_text (GTK_ENTRY (heightEntry),
                      g_markup_printf_escaped (
                          "%d", presentationAttributes.resolutionHeight));
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), heightEntry, 170,
                 (BUTTON_SIZE) + 20);

  GtkWidget *fps_label = gtk_label_new ("Frame Rate");
  gtk_label_set_markup (
      GTK_LABEL (fps_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "Frame Rate"));
  g_assert_nonnull (fps_label);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), fps_label, 30,
                 (BUTTON_SIZE * 3) + (BUTTON_SIZE / 4));

  GtkWidget *fps_combobox = gtk_combo_box_text_new ();
  g_assert_nonnull (fps_combobox);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (fps_combobox), -1,
                                  "30            ");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (fps_combobox), -1,
                                  "60            ");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (fps_combobox), -1,
                                  "Free          ");
  gtk_combo_box_set_active (GTK_COMBO_BOX (fps_combobox),
                            presentationAttributes.frameRate);
  g_signal_connect (fps_combobox, "changed",
                    G_CALLBACK (fps_combobox_changed), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout_presentation), fps_combobox, 120,
                 (BUTTON_SIZE * 3) - 5);

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                            fixed_layout_presentation,
                            gtk_label_new ("Presentation"));
}

void
create_settings_window (void)
{
  if (settingsWindow != NULL)
    return;

  original_presentationAttributes = presentationAttributes;

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

  create_presentation_page (notebook);

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

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), fixed_layout_controls,
                            gtk_label_new ("Controls"));

  gtk_fixed_put (GTK_FIXED (fixed_layout), notebook, 0, 0);
  GtkWidget *apply_button = gtk_button_new_with_label ("Apply");
  g_assert_nonnull (apply_button);
  g_signal_connect (apply_button, "clicked",
                    G_CALLBACK (apply_settings_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), apply_button, 05, 195);

  GtkWidget *cancel_button = gtk_button_new_with_label ("Cancel");
  g_assert_nonnull (cancel_button);
  g_signal_connect (cancel_button, "clicked",
                    G_CALLBACK (destroy_settings_window), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), cancel_button, 75, 195);

  gtk_container_add (GTK_CONTAINER (settingsWindow), fixed_layout);

  g_signal_connect (settingsWindow, "destroy",
                    G_CALLBACK (destroy_settings_window), NULL);

  gtk_widget_show_all (settingsWindow);
}

void
destroy_settings_window (void)
{
  presentationAttributes = original_presentationAttributes;
  gtk_widget_destroy (settingsWindow);
  settingsWindow = NULL;
}