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

gboolean isDebugMode = FALSE;

void
resize_main_window_canvas (void)
{
  printf ("Resizing %d - %d", presentationAttributes.resolutionWidth,
          presentationAttributes.resolutionHeight);


  gtk_widget_set_size_request (ginga_gui.canvas,
                               presentationAttributes.resolutionWidth,
                               presentationAttributes.resolutionHeight);                                                  

  destroy_settings_window ();
}

void
create_main_window (void)
{
  // Create application window.
  ginga_gui.toplevel_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (ginga_gui.toplevel_window);
  gtk_window_set_title (GTK_WINDOW (ginga_gui.toplevel_window),
                        PACKAGE_STRING);
  gtk_window_set_default_size (GTK_WINDOW (ginga_gui.toplevel_window),
                               ginga_gui.window_rect.w,
                               ginga_gui.window_rect.h);
  gtk_window_set_position (GTK_WINDOW (ginga_gui.toplevel_window),
                           GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (ginga_gui.toplevel_window), FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (ginga_gui.toplevel_window),
                                  ginga_gui.default_margin);

  ginga_gui.fixed_layout = gtk_fixed_new ();
  g_assert_nonnull (ginga_gui.fixed_layout);

  GtkWidget *icon = gtk_image_new_from_icon_name ("document-open",
                                                  GTK_ICON_SIZE_BUTTON);
  ginga_gui.open_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.open_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.open_button), icon);
  g_signal_connect (ginga_gui.open_button, "clicked",
                    G_CALLBACK (select_ncl_file_callback), NULL);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.open_button,
                 0, MENU_BOX_HEIGHT - 38);

  ginga_gui.file_entry = gtk_entry_new ();
  g_assert_nonnull (ginga_gui.file_entry);
  gtk_widget_set_size_request (ginga_gui.file_entry,
                               ginga_gui.window_rect.w - BUTTON_SIZE,
                               BUTTON_SIZE);
  gtk_entry_set_text (GTK_ENTRY (ginga_gui.file_entry),
                      g_get_user_special_dir (G_USER_DIRECTORY_DOCUMENTS));
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.file_entry,
                 BUTTON_SIZE, MENU_BOX_HEIGHT - 38);

  GtkWidget *canvas_separator
      = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_size_request (canvas_separator, ginga_gui.canvas_rect.w,
                               2);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), canvas_separator,
                 ginga_gui.canvas_rect.x, ginga_gui.canvas_rect.y - 5);

  // Create Drawing area
  ginga_gui.canvas = gtk_drawing_area_new ();
  g_assert_nonnull (ginga_gui.canvas);
  gtk_widget_set_app_paintable (ginga_gui.canvas, TRUE);
  g_signal_connect (ginga_gui.canvas, "draw", G_CALLBACK (draw_callback),
                    NULL);
  gtk_widget_set_size_request (ginga_gui.canvas, ginga_gui.canvas_rect.w,
                               ginga_gui.canvas_rect.h);

  ginga_gui.canvas_separator_bottom
      = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_size_request (ginga_gui.canvas_separator_bottom,
                               ginga_gui.canvas_rect.w, 2);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout),
                 ginga_gui.canvas_separator_bottom, 0,
                 ginga_gui.canvas_rect.y + ginga_gui.canvas_rect.h
                     + ginga_gui.default_margin);

  ginga_gui.log_view = gtk_text_view_new ();
  g_assert_nonnull (ginga_gui.log_view);
  gtk_widget_set_size_request (ginga_gui.log_view, ginga_gui.canvas_rect.w,
                               ginga_gui.canvas_rect.h);

  ginga_gui.notebook = gtk_notebook_new ();
  g_assert_nonnull (ginga_gui.notebook);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (ginga_gui.notebook), FALSE);
  gtk_notebook_append_page (GTK_NOTEBOOK (ginga_gui.notebook),
                            ginga_gui.canvas,
                            gtk_label_new ("Presentation"));
  gtk_notebook_append_page (GTK_NOTEBOOK (ginga_gui.notebook),
                            ginga_gui.log_view, gtk_label_new ("Log"));
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.notebook,
                 ginga_gui.canvas_rect.x, ginga_gui.canvas_rect.y);

  // ----- top-menu begin
  ginga_gui.menu_bar = gtk_menu_bar_new ();
  g_assert_nonnull (ginga_gui.menu_bar);

  // File
  GtkWidget *menu_file = gtk_menu_new ();
  g_assert_nonnull (menu_file);
  GtkWidget *menu_item_file = gtk_menu_item_new_with_label ("File");
  g_assert_nonnull (menu_item_file);
  GtkWidget *menu_item_open
      = gtk_menu_item_new_with_label ("Open  (Ctrl+O)");
  g_assert_nonnull (menu_item_open);
  GtkWidget *menu_item_quit
      = gtk_menu_item_new_with_label ("Quit  (Ctrl+Q)");
  g_assert_nonnull (menu_item_quit);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item_file), menu_file);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_file), menu_item_open);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_file), menu_item_quit);
  gtk_menu_shell_append (GTK_MENU_SHELL (ginga_gui.menu_bar),
                         menu_item_file);

  // Tools
  GtkWidget *menu_tool = gtk_menu_new ();
  g_assert_nonnull (menu_tool);
  GtkWidget *menu_item_tools = gtk_menu_item_new_with_label ("Tools");
  g_assert_nonnull (menu_item_tools);
  GtkWidget *menu_item_tvcontrol
      = gtk_menu_item_new_with_label ("TV Control  (Ctrl+R)");
  g_assert_nonnull (menu_item_tvcontrol);
  GtkWidget *menu_item_debug
      = gtk_menu_item_new_with_label ("Debug Mode  (Ctrl+D)");
  g_assert_nonnull (menu_item_debug);
  g_signal_connect (menu_item_tvcontrol, "activate",
                    G_CALLBACK (create_tvcontrol_window), NULL);
  g_signal_connect (menu_item_debug, "activate",
                    G_CALLBACK (enable_disable_debug), NULL);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item_tools), menu_tool);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_tool), menu_item_tvcontrol);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_tool), menu_item_debug);
  gtk_menu_shell_append (GTK_MENU_SHELL (ginga_gui.menu_bar),
                         menu_item_tools);

  // Help
  GtkWidget *menu_help = gtk_menu_new ();
  g_assert_nonnull (menu_help);
  GtkWidget *menu_item_help = gtk_menu_item_new_with_label ("Help");
  g_assert_nonnull (menu_item_help);
  GtkWidget *menu_item_about = gtk_menu_item_new_with_label ("About");
  g_assert_nonnull (menu_item_about);
  g_signal_connect (menu_item_about, "activate",
                    G_CALLBACK (create_about_window), NULL);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item_help), menu_help);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu_help), menu_item_about);
  gtk_menu_shell_append (GTK_MENU_SHELL (ginga_gui.menu_bar),
                         menu_item_help);

  GtkWidget *menu_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start (GTK_BOX (menu_box), ginga_gui.menu_bar, TRUE, TRUE,
                      3);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), menu_box, 0, 0);

  // ----- top-menu end

  GtkWidget *play_icon = gtk_image_new_from_icon_name (
      "media-playback-start", GTK_ICON_SIZE_BUTTON);
  g_assert_nonnull (play_icon);
  ginga_gui.play_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.play_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.play_button), play_icon);
  g_signal_connect (ginga_gui.play_button, "clicked",
                    G_CALLBACK (play_pause_ginga), NULL);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.play_button,
                 0, ginga_gui.controll_area_rect.y);

  GtkWidget *stop_icon = gtk_image_new_from_icon_name (
      "media-playback-stop", GTK_ICON_SIZE_BUTTON);
  g_assert_nonnull (stop_icon);
  ginga_gui.stop_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.stop_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.stop_button), stop_icon);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.stop_button,
                 BUTTON_SIZE, ginga_gui.controll_area_rect.y);

  ginga_gui.time_label = gtk_label_new ("00:00");
  gtk_label_set_markup (
      GTK_LABEL (ginga_gui.time_label),
      g_markup_printf_escaped ("<span font=\"13\"><b>\%s</b></span>",
                               "00:00"));
  g_assert_nonnull (ginga_gui.time_label);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.time_label,
                 (BUTTON_SIZE * 2) + 5,
                 ginga_gui.window_rect.h + MENU_BOX_HEIGHT
                     - (BUTTON_SIZE / 3));

  GtkWidget *fullscreen_icon = gtk_image_new_from_icon_name (
      "view-fullscreen", GTK_ICON_SIZE_BUTTON);
  g_assert_nonnull (fullscreen_icon);
  ginga_gui.fullscreen_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.fullscreen_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.fullscreen_button),
                        fullscreen_icon);
  g_signal_connect (ginga_gui.fullscreen_button, "clicked",
                    G_CALLBACK (set_fullscreen_mode), NULL);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout),
                 ginga_gui.fullscreen_button,
                 ginga_gui.window_rect.w - (BUTTON_SIZE * 2),
                 ginga_gui.controll_area_rect.y);

  GtkWidget *config_icon = gtk_image_new_from_icon_name (
      "emblem-system", GTK_ICON_SIZE_BUTTON);
  g_assert_nonnull (config_icon);
  ginga_gui.config_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.config_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.config_button), config_icon);
  g_signal_connect (ginga_gui.config_button, "clicked",
                    G_CALLBACK (create_settings_window), NULL);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout),
                 ginga_gui.config_button,
                 ginga_gui.window_rect.w - (BUTTON_SIZE),
                 ginga_gui.controll_area_rect.y);

  ginga_gui.volume_button = gtk_volume_button_new ();
  g_assert_nonnull (ginga_gui.volume_button);
  gtk_fixed_put (GTK_FIXED (ginga_gui.fixed_layout),
                 ginga_gui.volume_button,
                 ginga_gui.window_rect.w - (BUTTON_SIZE * 3),
                 ginga_gui.controll_area_rect.y);

  gtk_container_add (GTK_CONTAINER (ginga_gui.toplevel_window),
                     ginga_gui.fixed_layout);
  // Setup GTK+ callbacks.

  g_signal_connect (ginga_gui.toplevel_window, "key-press-event",
                    G_CALLBACK (key_press_event_callback), NULL);
  g_signal_connect (ginga_gui.toplevel_window, "key-release-event",
                    G_CALLBACK (key_release_event_callback), NULL);
  g_signal_connect (ginga_gui.toplevel_window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show_all (ginga_gui.toplevel_window);
}

void
destroy_main_window (void)
{
}

void
enable_disable_debug (void)
{
  isDebugMode = !isDebugMode;

  guint offset = 0;
  if (isDebugMode)
    {
      offset = 30;
      gtk_notebook_set_show_tabs (GTK_NOTEBOOK (ginga_gui.notebook), TRUE);
    }
  else
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK (ginga_gui.notebook), FALSE);

  gtk_fixed_move (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.play_button,
                  0, ginga_gui.controll_area_rect.y + offset);
  gtk_fixed_move (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.stop_button,
                  BUTTON_SIZE, ginga_gui.controll_area_rect.y + offset);
  gtk_fixed_move (GTK_FIXED (ginga_gui.fixed_layout), ginga_gui.time_label,
                  (BUTTON_SIZE * 2) + 5,
                  ginga_gui.window_rect.h + MENU_BOX_HEIGHT
                      - (BUTTON_SIZE / 3) + offset);
  gtk_fixed_move (GTK_FIXED (ginga_gui.fixed_layout),
                  ginga_gui.fullscreen_button,
                  ginga_gui.window_rect.w - (BUTTON_SIZE * 2),
                  ginga_gui.controll_area_rect.y + offset);
  gtk_fixed_move (GTK_FIXED (ginga_gui.fixed_layout),
                  ginga_gui.config_button,
                  ginga_gui.window_rect.w - (BUTTON_SIZE),
                  ginga_gui.controll_area_rect.y + offset);
  gtk_fixed_move (GTK_FIXED (ginga_gui.fixed_layout),
                  ginga_gui.volume_button,
                  ginga_gui.window_rect.w - (BUTTON_SIZE * 3),
                  ginga_gui.controll_area_rect.y + offset);
  gtk_fixed_move (GTK_FIXED (ginga_gui.fixed_layout),
                  ginga_gui.canvas_separator_bottom, 0,
                  ginga_gui.canvas_rect.y + ginga_gui.canvas_rect.h
                      + ginga_gui.default_margin + offset);

  gtk_window_resize (GTK_WINDOW (ginga_gui.toplevel_window),
                     ginga_gui.window_rect.w, ginga_gui.window_rect.h);
}

void
select_ncl_file_callback (GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new (
      "Open File", GTK_WINDOW (ginga_gui.toplevel_window),
      GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open",
      GTK_RESPONSE_ACCEPT, NULL);

  gint res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;
      GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
      filename = gtk_file_chooser_get_filename (chooser);

      gchar *ext = strrchr (filename, '.');

      if (g_strcmp0 (ext, ".ncl"))
        {
          GtkWidget *dialog_error = gtk_message_dialog_new (
              GTK_WINDOW (ginga_gui.toplevel_window),
              GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR,
              GTK_BUTTONS_CLOSE, "Error reading “%s”: %s", filename,
              "is a invalid formart file.");
          gtk_dialog_run (GTK_DIALOG (dialog_error));
          gtk_widget_destroy (dialog_error);
        }
      else
        {
          gtk_entry_set_text (GTK_ENTRY (ginga_gui.file_entry), filename);
        }

      g_free (filename);
    }

  gtk_widget_destroy (dialog);
}

void
play_pause_ginga (void)
{
  ginga_gui.playMode = !ginga_gui.playMode;
  GtkWidget *play_icon = gtk_image_new_from_icon_name (
      "media-playback-start", GTK_ICON_SIZE_BUTTON);
  if (ginga_gui.playMode)
    play_icon = gtk_image_new_from_icon_name ("media-playback-pause",
                                              GTK_ICON_SIZE_BUTTON);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.play_button), play_icon);
}