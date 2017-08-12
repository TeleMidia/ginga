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
GtkWidget *gingaView = NULL;
GtkWidget *sideFrame = NULL;
GtkWidget *fileEntry = NULL;
GtkWidget *playButton = NULL;
GtkWidget *stopButton = NULL;
GtkWidget *openButton = NULL;
GtkWidget *fullscreenButton = NULL;
GtkWidget *volumeButton = NULL;
GtkWidget *settingsButton = NULL;
GtkWidget *debugView = NULL;
GtkWidget *sideView = NULL;
GtkWidget *infoBar = NULL;
GtkWidget *toolBoxPopOver = NULL;
GtkWidget *optBoxPopOver = NULL;

gboolean isDebugMode = FALSE;
gboolean inPlayMode = FALSE;
gboolean destroyWindowToResize = FALSE;
gboolean isCrtlModifierActive = FALSE;

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
show_toolbox ()
{
  gtk_popover_popup (GTK_POPOVER (toolBoxPopOver));
}

void
show_optbox ()
{
  gtk_popover_popup (GTK_POPOVER (optBoxPopOver));
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
                                  5);

  GError *error = NULL;
  GtkCssProvider *css_provider = gtk_css_provider_get_default ();
  GFile *file = g_file_new_for_path (
      g_strconcat (executableFolder, "style/light.css", NULL));
  if (g_file_query_exists (file, NULL))
    {
      gtk_css_provider_load_from_file (css_provider, file, &error);
      if (error == NULL)
        {
          gtk_style_context_add_provider_for_screen (
              gdk_display_get_default_screen (gdk_display_get_default ()),
              GTK_STYLE_PROVIDER (css_provider),
              GTK_STYLE_PROVIDER_PRIORITY_USER);
        }
    }

  GtkWidget *header_bar = gtk_header_bar_new ();
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header_bar), true);

  fileEntry = gtk_entry_new ();
  g_assert_nonnull (fileEntry);
  gtk_widget_set_size_request (fileEntry, 400, 32);

  GtkWidget *open_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/openfile-icon.png", NULL));
  openButton = gtk_button_new ();
  g_assert_nonnull (openButton);
  gtk_button_set_image (GTK_BUTTON (openButton), open_icon);
  gtk_widget_set_has_tooltip(openButton, true);
  gtk_widget_set_tooltip_text (openButton, "Open file");
  g_signal_connect (openButton, "clicked",
                    G_CALLBACK (select_ncl_file_callback), NULL);

  GtkWidget *play_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/play-icon.png", NULL));
  g_assert_nonnull (play_icon);
  playButton = gtk_button_new ();
  g_assert_nonnull (playButton);
  gtk_button_set_image (GTK_BUTTON (playButton), play_icon);
  g_signal_connect (playButton, "clicked",
                    G_CALLBACK (play_pause_button_callback), NULL);

  GtkWidget *stop_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/stop-icon.png", NULL));
  g_assert_nonnull (stop_icon);
  stopButton = gtk_button_new ();
  g_assert_nonnull (stopButton);
  gtk_button_set_image (GTK_BUTTON (stopButton), stop_icon);
  g_signal_connect (stopButton, "clicked",
                    G_CALLBACK (stop_button_callback), NULL);

  GtkWidget *fullscreen_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/fullscreen-icon.png", NULL));
  g_assert_nonnull (fullscreen_icon);
  fullscreenButton = gtk_button_new ();
  g_assert_nonnull (fullscreenButton);
  gtk_button_set_image (GTK_BUTTON (fullscreenButton),
                        fullscreen_icon);
  gtk_widget_set_has_tooltip(fullscreenButton, true);
  gtk_widget_set_tooltip_text (fullscreenButton, "Fullscreen");                      
  g_signal_connect (fullscreenButton, "clicked",
                    G_CALLBACK (set_fullscreen_mode), NULL);

  volumeButton = gtk_volume_button_new ();
  g_assert_nonnull (volumeButton);

  GtkWidget *config_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/settings-icon.png", NULL));
  g_assert_nonnull (config_icon);
  settingsButton = gtk_button_new ();
  g_assert_nonnull (settingsButton);
  gtk_button_set_image (GTK_BUTTON (settingsButton), config_icon);
  gtk_widget_set_has_tooltip(settingsButton, true);
  gtk_widget_set_tooltip_text (settingsButton, "Preferences"); 
  g_signal_connect (settingsButton, "clicked",
                    G_CALLBACK (show_optbox), NULL);

  GtkWidget *remote_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/remote-icon.png", NULL));
  g_assert_nonnull (remote_icon);
  GtkWidget *remote_button = gtk_button_new ();
  g_assert_nonnull (remote_button);
  gtk_button_set_image (GTK_BUTTON (remote_button), remote_icon);
  gtk_widget_set_has_tooltip(remote_button, true);
  gtk_widget_set_tooltip_text (remote_button, "Remote Control"); 
  g_signal_connect (remote_button, "clicked",
                    G_CALLBACK (create_tvcontrol_window), NULL);

  GtkWidget *tools_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/tools-icon.png", NULL));
  g_assert_nonnull (tools_icon);
  GtkWidget *tools_button = gtk_button_new ();
  g_assert_nonnull (tools_button);
  gtk_button_set_image (GTK_BUTTON (tools_button), tools_icon);
  gtk_widget_set_has_tooltip(tools_button, true);
  gtk_widget_set_tooltip_text (tools_button, "Tools");
  g_signal_connect (tools_button, "clicked", G_CALLBACK (show_toolbox),
                    NULL);

  GtkWidget *debug_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/debug-icon.png", NULL));
  g_assert_nonnull (debug_icon);
  GtkWidget *debug_button = gtk_button_new ();
  g_assert_nonnull (debug_button);
  gtk_button_set_image (GTK_BUTTON (debug_button), debug_icon);
  gtk_widget_set_has_tooltip(debug_button, true);
  gtk_widget_set_tooltip_text (debug_button, "Debug");
  g_signal_connect (debug_button, "clicked",
                    G_CALLBACK (enable_disable_debug), NULL);

  /* begin tool box */
  GtkWidget *tool_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  g_assert_nonnull (tool_box);

  gtk_box_pack_start (GTK_BOX (tool_box), remote_button, false, false, 0);
  gtk_box_pack_start (GTK_BOX (tool_box), debug_button, false, false, 0);
  toolBoxPopOver = gtk_popover_new (tools_button);
  g_assert_nonnull (toolBoxPopOver);

  gtk_container_add (GTK_CONTAINER (toolBoxPopOver), tool_box);
  g_object_set (tool_box, "margin", 5, NULL);
  gtk_widget_show_all (tool_box);
  /* end tool box */

  GtkWidget *hist_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/history-icon.png", NULL));
  GtkWidget *hist_button = gtk_button_new ();
  gtk_button_set_image (GTK_BUTTON (hist_button), hist_icon);
    gtk_widget_set_has_tooltip(hist_button, true);
  gtk_widget_set_tooltip_text (hist_button, "Historic");
  g_assert_nonnull (hist_button);
  /*g_signal_connect (hist_button, "clicked",
                    G_CALLBACK (show_toolbox), NULL);  */

  /* begin option box */

  GtkWidget *aspect_combobox = gtk_combo_box_text_new ();
  g_assert_nonnull (aspect_combobox);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "TV (4:3)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "HDTV (16:9)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "LCD (16:10)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "Free");                                
  gtk_combo_box_set_active (GTK_COMBO_BOX (aspect_combobox),
                            presentationAttributes.aspectRatio);

  GtkWidget *fps_combobox = gtk_combo_box_text_new ();
  g_assert_nonnull (fps_combobox);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (fps_combobox), -1,
                                  "30");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (fps_combobox), -1,
                                  "60");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (fps_combobox), -1,
                                  "Go Horse!");
  gtk_combo_box_set_active (GTK_COMBO_BOX (fps_combobox), 0);

  GtkWidget *theme_combobox = gtk_combo_box_text_new ();
  g_assert_nonnull (theme_combobox);
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (theme_combobox), -1,
                                  "Light");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (theme_combobox), -1,
                                  "Dark");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (theme_combobox), -1,
                                  "Marine");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (theme_combobox), -1,
                                  "Crystal");                                
  gtk_combo_box_set_active (GTK_COMBO_BOX (theme_combobox), 0);
  /* g_signal_connect (aspect_combobox, "changed",
                     G_CALLBACK (aspect_combobox_changed), NULL); */

  GtkWidget *opt_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  g_assert_nonnull (opt_box);

  gtk_box_pack_start (GTK_BOX (opt_box), gtk_label_new ("Aspect Ratio:"),
                      false, false, 0);
  gtk_box_pack_start (GTK_BOX (opt_box), aspect_combobox, false, true, 0);
  gtk_box_pack_start (GTK_BOX (opt_box), gtk_label_new ("Frame Ratte:"),
                      false, false, 0);
  gtk_box_pack_start (GTK_BOX (opt_box), fps_combobox, false, true, 0);
  gtk_box_pack_start (GTK_BOX (opt_box), gtk_label_new ("GUI Theme:"),
                      false, false, 0);
  gtk_box_pack_start (GTK_BOX (opt_box), theme_combobox, false, true, 0);

  optBoxPopOver = gtk_popover_new (settingsButton);
  g_assert_nonnull (optBoxPopOver);
  // gtk_widget_set_size_request (toolBoxPopOver, 200, 100);

  gtk_container_add (GTK_CONTAINER (optBoxPopOver), opt_box);
  g_object_set (opt_box, "margin", 5, NULL);
  gtk_widget_show_all (opt_box);

  /* end option box */

  GtkWidget *sep1 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep1, 1, 30);

  GtkWidget *sep2 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep2, 1, 30);

  GtkWidget *sep3 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep3, 1, 30);

  GtkWidget *file_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);

  gtk_box_pack_start (GTK_BOX (file_box), openButton, false,
                      false, 0);

  gtk_box_pack_start (GTK_BOX (file_box), fileEntry, false,
                      false, 0);

  gtk_box_pack_start (GTK_BOX (file_box), hist_button, false, false, 0);

  gtk_header_bar_set_custom_title (GTK_HEADER_BAR (header_bar), file_box);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar),
                           settingsButton);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar),
                           fullscreenButton);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar), sep1);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar),
                             playButton);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar),
                             stopButton);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), sep2);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar),
                             volumeButton);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), tools_button);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), sep3);

  gtk_window_set_titlebar (GTK_WINDOW (mainWindow), header_bar);

  // Create Drawing area
  gingaView = gtk_drawing_area_new ();
  g_assert_nonnull (gingaView);
  gtk_widget_set_app_paintable (gingaView, TRUE);
  g_signal_connect (gingaView, "draw", G_CALLBACK (draw_callback),
                    NULL);
  gtk_widget_set_size_request (gingaView,
                               presentationAttributes.resolutionWidth,
                               presentationAttributes.resolutionHeight);

#if GTK_CHECK_VERSION(3, 8, 0)
  gtk_widget_add_tick_callback (
      gingaView, (GtkTickCallback)update_draw_callback, NULL, NULL);
#else
  g_timeout_add (1000 / 60, (GSourceFunc)update_draw_callback,
                 gingaView);
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
                               presentationAttributes.resolutionWidth, 140);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (debugView), false);
  gtk_widget_set_margin_top (debugView, 5);

  GtkWidget *button_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
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

  /* begin side view */

  sideView = gtk_notebook_new ();
  g_assert_nonnull (sideView);
  gtk_widget_set_size_request (sideView, (BUTTON_SIZE * 4) + 15, 100);
  gtk_widget_set_margin_start (sideView, 5);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (sideView), false);

  button_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/window-icon.png", NULL));
  g_assert_nonnull (button_icon);
  GtkWidget *sidebar_win_button = gtk_button_new ();
  g_assert_nonnull (sidebar_win_button);
  gtk_button_set_image (GTK_BUTTON (sidebar_win_button), button_icon);
  g_signal_connect (sidebar_win_button, "clicked",
                    G_CALLBACK (change_tvcontrol_to_window_mode), NULL);

  button_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
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
  gtk_box_pack_start (GTK_BOX (v_box), gingaView, true, true, 0);

  gtk_box_pack_start (GTK_BOX (h_box), v_box, true, true, 0);
  gtk_box_pack_start (GTK_BOX (h_box), sideView, false, true, 0);

  /* end side view */

  GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
  g_assert_nonnull (vpaned);
  // gtk_widget_set_size_request (vpaned, 200, -1);

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
          gtk_entry_set_text (GTK_ENTRY (fileEntry), filename);
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
  inPlayMode = false;
  GtkWidget *play_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/play-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (playButton), play_icon);
  gtk_widget_set_sensitive (fileEntry, true);
  gtk_widget_set_sensitive (openButton, true);

  gtk_widget_show_now (infoBar);
}

void
play_pause_button_callback (void)
{
  inPlayMode = !inPlayMode;
  GtkWidget *play_icon = gtk_image_new_from_file (
      g_strconcat (executableFolder,
                   "icons/light-theme/play-icon.png", NULL));
  if (inPlayMode)
    {
      play_icon = gtk_image_new_from_file (
          g_strconcat (executableFolder,
                       "icons/light-theme/pause-icon.png", NULL));
      gtk_widget_set_sensitive (fileEntry, false);
      gtk_widget_set_sensitive (openButton, false);
      const gchar *file
          = gtk_entry_get_text (GTK_ENTRY (fileEntry));
      // Start Ginga.
      GINGA->start (file);
    }
  else
    {
      gtk_widget_set_sensitive (fileEntry, true);
      gtk_widget_set_sensitive (openButton, true);
    }
  gtk_button_set_image (GTK_BUTTON (playButton), play_icon);
}