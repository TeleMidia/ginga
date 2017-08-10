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

GtkWidget *sideFrame = NULL;
GtkWidget *debugView = NULL;
GtkWidget *sideView = NULL;
GtkWidget *infoBar = NULL;

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
hide_sideview ()
{
  gtk_widget_hide (sideView);
}

void
hide_debugview ()
{
  isDebugMode = FALSE;
  gtk_widget_hide (debugView);
}

void
hide_infobar ()
{
  gtk_widget_hide (infoBar);
}

void
change_tvcontrol_to_window_mode ()
{
  gtk_widget_hide (sideView);
  tvcontrolAsSidebar = FALSE;
  create_tvcontrol_window ();
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

  GError *error = NULL;
  GtkCssProvider *css_provider = gtk_css_provider_get_default ( );
  GFile *file = g_file_new_for_path (
      g_strconcat (ginga_gui.executable_folder, "style/dark.css", NULL));
  if (g_file_query_exists (file, NULL))
    {
      gtk_css_provider_load_from_file (css_provider, file, &error);
      if(error==NULL)
         printf("CARREGOU DE BOAS! \n");
    }

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
  g_signal_connect (debug_button, "clicked",
                    G_CALLBACK (enable_disable_debug), NULL);

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

  /* begin debug view   */

  GtkWidget *textArea = gtk_text_view_new ();
  g_assert_nonnull (textArea);
  GtkWidget *scrolledWLog = gtk_scrolled_window_new (NULL, NULL);
  g_assert_nonnull (scrolledWLog);
  gtk_container_add (GTK_CONTAINER (scrolledWLog), textArea);

  debugView = gtk_notebook_new ();
  g_assert_nonnull (debugView);
  gtk_widget_set_size_request (debugView,
                               presentationAttributes.resolutionWidth, 160);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (debugView), false);
  gtk_widget_set_margin_top (debugView, 5);

  GtkWidget *button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/bottomhide-icon.png", NULL));
  g_assert_nonnull (button_icon);
  GtkWidget *debug_hide_button = gtk_button_new ();
  g_assert_nonnull (debug_hide_button);
  gtk_button_set_image (GTK_BUTTON (debug_hide_button), button_icon);
  g_signal_connect (debug_hide_button, "clicked",
                    G_CALLBACK (hide_debugview), NULL);

  GtkWidget *debug_headbar = gtk_header_bar_new ();
  g_assert_nonnull (debug_headbar);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (debug_headbar),
                             gtk_label_new ("Console"));
  gtk_header_bar_pack_end (GTK_HEADER_BAR (debug_headbar),
                           debug_hide_button);
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (debug_headbar),
                                        false);

  GtkWidget *debug_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  g_assert_nonnull (debug_box);

  gtk_box_pack_start (GTK_BOX (debug_box), debug_headbar, false, true, 0);
  gtk_box_pack_start (GTK_BOX (debug_box), scrolledWLog, true, true, 0);

  gtk_notebook_append_page (GTK_NOTEBOOK (debugView), debug_box,
                            gtk_label_new ("Console"));

  /* end debug view */

  sideView = gtk_notebook_new ();
  g_assert_nonnull (sideView);
  gtk_widget_set_size_request (sideView, (BUTTON_SIZE * 4) + 15, -1);
  gtk_widget_set_margin_start (sideView, 5);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (sideView), false);

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/window-icon.png", NULL));
  g_assert_nonnull (button_icon);
  GtkWidget *sidebar_win_button = gtk_button_new ();
  g_assert_nonnull (sidebar_win_button);
  gtk_button_set_image (GTK_BUTTON (sidebar_win_button), button_icon);
  g_signal_connect (sidebar_win_button, "clicked",
                    G_CALLBACK (change_tvcontrol_to_window_mode), NULL);

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/rightride-icon.png", NULL));
  g_assert_nonnull (button_icon);
  GtkWidget *sidebar_hide_button = gtk_button_new ();
  g_assert_nonnull (sidebar_hide_button);
  gtk_button_set_image (GTK_BUTTON (sidebar_hide_button), button_icon);
  g_signal_connect (sidebar_hide_button, "clicked",
                    G_CALLBACK (hide_sideview), NULL);

  GtkWidget *tvcontrol_headbar = gtk_header_bar_new ();
  g_assert_nonnull (tvcontrol_headbar);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (tvcontrol_headbar),
                             sidebar_win_button);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (tvcontrol_headbar),
                           sidebar_hide_button);
  gtk_header_bar_set_title (GTK_HEADER_BAR (tvcontrol_headbar), "Control");
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (tvcontrol_headbar),
                                        false);

  GtkWidget *tvcontrol_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  g_assert_nonnull (tvcontrol_box);

  gtk_box_pack_start (GTK_BOX (tvcontrol_box), tvcontrol_headbar, false,
                      true, 0);
  gtk_box_pack_start (GTK_BOX (tvcontrol_box),
                      create_tvremote_buttons (10, 10), false, true, 0);

  gtk_notebook_append_page (GTK_NOTEBOOK (sideView), tvcontrol_box,
                            gtk_label_new ("Control"));

  GtkWidget *h_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  g_assert_nonnull (h_box);

  infoBar = gtk_info_bar_new ();
  g_assert_nonnull (infoBar);
  GtkWidget *content_area
      = gtk_info_bar_get_content_area (GTK_INFO_BAR (infoBar));
  gtk_container_add (GTK_CONTAINER (content_area),
                     gtk_label_new ("Invalid file extension"));
  gtk_info_bar_set_show_close_button (GTK_INFO_BAR (infoBar), true);
  gtk_info_bar_set_message_type (GTK_INFO_BAR (infoBar), GTK_MESSAGE_ERROR);
  g_signal_connect (GTK_INFO_BAR (infoBar), "response",
                    G_CALLBACK (hide_infobar), NULL);

  GtkWidget *v_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  g_assert_nonnull (v_box);

  gtk_box_pack_start (GTK_BOX (v_box), infoBar, false, true, 0);
  gtk_box_pack_start (GTK_BOX (v_box), ginga_gui.canvas, true, true, 0);

  gtk_box_pack_start (GTK_BOX (h_box), v_box, true, true, 0);
  gtk_box_pack_start (GTK_BOX (h_box), sideView, false, true, 0);

  GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
  g_assert_nonnull (vpaned);
  gtk_widget_set_size_request (vpaned, 200, -1);

  gtk_paned_pack1 (GTK_PANED (vpaned), h_box, true, true);
  gtk_paned_pack2 (GTK_PANED (vpaned), debugView, true, false);

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

  gtk_widget_hide (debugView);
  gtk_widget_hide (sideView);
  gtk_widget_hide (infoBar);
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
  isDebugMode = !isDebugMode;

  if (isDebugMode)
    gtk_widget_show (GTK_WIDGET (debugView));
  else
    gtk_widget_hide (GTK_WIDGET (debugView));
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

  gtk_widget_show_now (infoBar);
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