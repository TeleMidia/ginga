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

GtkWidget *mainWindow = NULL;

gboolean isDebugMode = FALSE;
gboolean destroyWindowToResize = FALSE;
gboolean isCrtlModifierActive = FALSE;

void
resize_main_window_callback (GtkWidget *widget, gpointer data)
{
  /* int w, h;
   w = gtk_widget_get_allocated_width (widget);
   h = gtk_widget_get_allocated_height (ginga_gui.canvas);

   gtk_widget_set_size_request (ginga_gui.canvas, w - 10, h); */
}

void
keyboard_callback (GtkWidget *widget, GdkEventKey *e, gpointer type)
{
  const char *key;
  gboolean free_key = FALSE;

  switch (e->keyval)
    {
    case GDK_KEY_Escape: /* quit */
      if (isFullScreenMode)
        set_unfullscreen_mode ();
      return;
    case GDK_KEY_Meta_L:
    case GDK_KEY_Meta_R:
    case GDK_KEY_Control_R:
    case GDK_KEY_Control_L:
      if (g_strcmp0 ((const char *)type, "press") == 0)
        isCrtlModifierActive = TRUE;
      else
        isCrtlModifierActive = FALSE;
      return;
    case GDK_KEY_F:
    case GDK_KEY_f:
      if (isCrtlModifierActive && !isFullScreenMode)
        set_fullscreen_mode ();
      return;
    case GDK_KEY_R:
    case GDK_KEY_r:
      if (isCrtlModifierActive)
        create_tvcontrol_window ();
      return;
    case GDK_KEY_D:
    case GDK_KEY_d:
      if (isCrtlModifierActive)
        enable_disable_debug ();
      return;
    case GDK_KEY_asterisk:
      key = "*";
      break;
    case GDK_KEY_numbersign:
      key = "#";
      break;
    case GDK_KEY_Return:
      key = "ENTER";
      break;
    case GDK_KEY_F1:
      key = "RED";
      break;
    case GDK_KEY_F2:
      key = "GREEN";
      break;
    case GDK_KEY_F3:
      key = "BLUE";
      break;
    case GDK_KEY_F4:
      key = "YELLOW";
      break;
    case GDK_KEY_F5:
      key = "INFO";
      break;
    case GDK_KEY_Down:
      key = "CURSOR_DOWN";
      break;
    case GDK_KEY_Left:
      key = "CURSOR_LEFT";
      break;
    case GDK_KEY_Right:
      key = "CURSOR_RIGHT";
      break;
    case GDK_KEY_Up:
      key = "CURSOR_UP";
      break;
    default:
      key = gdk_keyval_name (e->keyval);
      if (strlen (key) > 1)
        {
          key = g_utf8_strup (key, -1);
          free_key = TRUE;
        }
      break;
    }

  GINGA->send_key (std::string (key),
                   g_strcmp0 ((const char *)type, "press") == 0);
  /*  if (free_key)
      g_free (key); */
}

void
create_main_window (void)
{
  // Create application window.
  mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (mainWindow);
  gtk_window_set_title (GTK_WINDOW (mainWindow), "Ginga");
  gtk_window_set_default_size (GTK_WINDOW (mainWindow),
                               presentationAttributes.resolutionWidth,
                               presentationAttributes.resolutionHeight);
  gtk_window_set_position (GTK_WINDOW (mainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (mainWindow), true);
  gtk_container_set_border_width (GTK_CONTAINER (mainWindow),
                                  ginga_gui.default_margin);

  GtkWidget *header_bar = gtk_header_bar_new ();
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header_bar), true);

  ginga_gui.file_entry = gtk_entry_new ();
  g_assert_nonnull (ginga_gui.file_entry);
  gtk_widget_set_size_request (ginga_gui.file_entry, 400, 32);

  GtkWidget *open_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/openfile-icon.png", NULL));
  ginga_gui.open_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.open_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.open_button), open_icon);
  g_signal_connect (ginga_gui.open_button, "clicked",
                    G_CALLBACK (select_ncl_file_callback), NULL);

  GtkWidget *play_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/play-icon.png", NULL));
  g_assert_nonnull (play_icon);
  ginga_gui.play_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.play_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.play_button), play_icon);
  g_signal_connect (ginga_gui.play_button, "clicked",
                    G_CALLBACK (play_pause_button_callback), NULL);

  GtkWidget *stop_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/stop-icon.png", NULL));
  g_assert_nonnull (stop_icon);
  ginga_gui.stop_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.stop_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.stop_button), stop_icon);
  g_signal_connect (ginga_gui.stop_button, "clicked",
                    G_CALLBACK (stop_button_callback), NULL);

  GtkWidget *fullscreen_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/fullscreen-icon.png", NULL));
  g_assert_nonnull (fullscreen_icon);
  ginga_gui.fullscreen_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.fullscreen_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.fullscreen_button),
                        fullscreen_icon);
  g_signal_connect (ginga_gui.fullscreen_button, "clicked",
                    G_CALLBACK (set_fullscreen_mode), NULL);

  ginga_gui.volume_button = gtk_volume_button_new ();
  g_assert_nonnull (ginga_gui.volume_button);

  GtkWidget *config_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  g_assert_nonnull (config_icon);
  ginga_gui.config_button = gtk_button_new ();
  g_assert_nonnull (ginga_gui.config_button);
  gtk_button_set_image (GTK_BUTTON (ginga_gui.config_button), config_icon);
  g_signal_connect (ginga_gui.config_button, "clicked",
                    G_CALLBACK (create_settings_window), NULL);

  GtkWidget *remote_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/remote-icon.png", NULL));
  g_assert_nonnull (remote_icon);
  GtkWidget *remote_button = gtk_button_new ();
  g_assert_nonnull (remote_button);
  gtk_button_set_image (GTK_BUTTON (remote_button), remote_icon);
  g_signal_connect (remote_button, "clicked",
                    G_CALLBACK (create_tvcontrol_window), NULL);

  GtkWidget *debug_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/debug-icon.png", NULL));
  g_assert_nonnull (debug_icon);
  GtkWidget *debug_button = gtk_button_new ();
  g_assert_nonnull (debug_button);
  gtk_button_set_image (GTK_BUTTON (debug_button), debug_icon);
  /* g_signal_connect (debug_button, "clicked",
                     G_CALLBACK (create_tvcontrol_window), NULL);   */

  GtkWidget *hist_button = gtk_menu_button_new ();

  /*gtk_menu_button_set_popup ( GTK_MENU_BUTTON(hist_button),
                          menu_file); */

  gtk_menu_button_set_direction (GTK_MENU_BUTTON (hist_button),
                                 GTK_ARROW_DOWN);

  GtkWidget *sep1 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep1, 1, 30);

  GtkWidget *sep2 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep2, 1, 30);

  GtkWidget *sep3 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep3, 1, 30);

  GtkWidget *file_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);

  gtk_box_pack_start (GTK_BOX (file_box), ginga_gui.open_button, false,
                      false, 0);

  gtk_box_pack_start (GTK_BOX (file_box), ginga_gui.file_entry, false,
                      false, 0);

  gtk_box_pack_start (GTK_BOX (file_box), hist_button, false, false, 0);

  gtk_header_bar_set_custom_title (GTK_HEADER_BAR (header_bar), file_box);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar),
                           ginga_gui.config_button);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar),
                           ginga_gui.fullscreen_button);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar),
                           ginga_gui.volume_button);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar), sep1);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar),
                             ginga_gui.play_button);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar),
                             ginga_gui.stop_button);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), sep2);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), remote_button);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), debug_button);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), sep3);

  gtk_window_set_titlebar (GTK_WINDOW (mainWindow), header_bar);

  // Create Drawing area
  ginga_gui.canvas = gtk_drawing_area_new ();
  g_assert_nonnull (ginga_gui.canvas);
  gtk_widget_set_app_paintable (ginga_gui.canvas, TRUE);
  g_signal_connect (ginga_gui.canvas, "draw", G_CALLBACK (draw_callback),
                    NULL);
  gtk_widget_set_size_request (ginga_gui.canvas,
                               presentationAttributes.resolutionWidth,
                               presentationAttributes.resolutionHeight);

#if GTK_CHECK_VERSION(3, 8, 0)
  gtk_widget_add_tick_callback (
      ginga_gui.canvas, (GtkTickCallback)update_draw_callback, NULL, NULL);
#else
  g_timeout_add (1000 / 60, (GSourceFunc)update_draw_callback,
                 ginga_gui.canvas);
#endif

  /* ginga_gui.log_view = gtk_text_view_new ();
   g_assert_nonnull (ginga_gui.log_view);
   gtk_widget_set_size_request (ginga_gui.log_view,
                                presentationAttributes.resolutionWidth,
   100);

  */

  GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);

  gtk_widget_set_size_request (vpaned, 200, -1);

  gtk_paned_pack1 (GTK_PANED (vpaned), ginga_gui.canvas, true, true);
  // gtk_paned_pack2 (GTK_PANED (vpaned), ginga_gui.log_view, true, false);

  gtk_container_add (GTK_CONTAINER (mainWindow), vpaned);

  /* g_signal_connect (flow_box, "check-resize",
                     G_CALLBACK (resize_main_window_callback), NULL); */
  g_signal_connect (mainWindow, "key-press-event",
                    G_CALLBACK (keyboard_callback), (void *)"press");
  g_signal_connect (mainWindow, "key-release-event",
                    G_CALLBACK (keyboard_callback), (void *)"release");
  g_signal_connect (mainWindow, "destroy", G_CALLBACK (destroy_main_window),
                    NULL);

  gtk_widget_show_all (mainWindow);
}

void
destroy_main_window (void)
{
//  stop_application ();
  gtk_widget_destroy (mainWindow);
  mainWindow = NULL;
  if (!destroyWindowToResize)
    {
      gtk_main_quit ();
    }
}

void
enable_disable_debug (void)
{
}

void
select_ncl_file_callback (GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new (
      "Open File", GTK_WINDOW (mainWindow), GTK_FILE_CHOOSER_ACTION_OPEN,
      "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

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
              GTK_WINDOW (mainWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
              GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
              "Error reading “%s”: %s", filename,
              "is a invalid formart file.");
          gtk_dialog_run (GTK_DIALOG (dialog_error));
          gtk_widget_destroy (dialog_error);
        }
      else
        {
          gtk_entry_set_text (GTK_ENTRY (ginga_gui.file_entry), filename);
          presentationAttributes.lastFileName = filename;
        }

      g_free (filename);
    }

  gtk_widget_destroy (dialog);
  save_settings ();
}

void
stop_button_callback (void)
{
  ginga_gui.playMode = false;
  GtkWidget *play_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/play-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (ginga_gui.play_button), play_icon);
  gtk_widget_set_sensitive (ginga_gui.file_entry, true);
  gtk_widget_set_sensitive (ginga_gui.open_button, true);
}

void
play_pause_button_callback (void)
{
  ginga_gui.playMode = !ginga_gui.playMode;
  GtkWidget *play_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/play-icon.png", NULL));
  if (ginga_gui.playMode)
    {
      play_icon = gtk_image_new_from_file (
          g_strconcat (ginga_gui.executable_folder,
                       "icons/light-theme/pause-icon.png", NULL));
      gtk_widget_set_sensitive (ginga_gui.file_entry, false);
      gtk_widget_set_sensitive (ginga_gui.open_button, false);
      const gchar *file
          = gtk_entry_get_text (GTK_ENTRY (ginga_gui.file_entry));
      // Start Ginga.
      GINGA->start (file);
    }
  else
    {
      gtk_widget_set_sensitive (ginga_gui.file_entry, true);
      gtk_widget_set_sensitive (ginga_gui.open_button, true);
    }
  gtk_button_set_image (GTK_BUTTON (ginga_gui.play_button), play_icon);
}