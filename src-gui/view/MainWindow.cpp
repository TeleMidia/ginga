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
#include "aux-glib.h"

GtkWidget *mainWindow = NULL;
GtkWidget *gingaView = NULL;
GtkWidget *sideFrame = NULL;
GtkWidget *fileEntry = NULL;
GtkWidget *playButton = NULL;
GtkWidget *stopButton = NULL;
GtkWidget *openButton = NULL;
GtkWidget *histButton = NULL;
GtkWidget *debugButton = NULL;
GtkWidget *toolsButton = NULL;
GtkWidget *remoteButton = NULL;
GtkWidget *fullscreenButton = NULL;
GtkWidget *volumeButton = NULL;
GtkWidget *settingsButton = NULL;
GtkWidget *aboutButton = NULL;
GtkWidget *helpButton = NULL;
GtkWidget *bigpictureButton = NULL;
GtkWidget *sidebarHideButton = NULL;
GtkWidget *sidebarWinButton = NULL;
GtkWidget *consoleHideButton = NULL;
GtkWidget *historicBox = NULL;
GtkWidget *debugView = NULL;
GtkWidget *sideView = NULL;
GtkWidget *tvRemoteView = NULL;
GtkWidget *tvcontrolBox = NULL;
GtkWidget *infoBar = NULL;
GtkWidget *toolBoxPopOver = NULL;
GtkWidget *optBoxPopOver = NULL;
GtkWidget *historicBoxPopOver = NULL;
GtkWidget *mainBox = NULL;
GtkWidget *visualDebugSwitch = NULL;
GtkTextBuffer *consoleTxtBuffer = NULL;

gboolean isDebugMode = FALSE;
gboolean visualDebugEnabled = FALSE;
gboolean needShowSideBar = FALSE;
gboolean needShowErrorBar = FALSE;
gboolean inPlayMode = FALSE;
gboolean isCrtlModifierActive = FALSE;

PresentationAttributes presentationAttributes;

void
show_ginga_update_alertbox ()
{
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget *dialog = gtk_message_dialog_new (
      GTK_WINDOW (mainWindow), flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
      "A new version of Ginga Software is\navaible at "
      "http://www.ginga.org.br");
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void
update_main_window ()
{
  gtk_widget_show_all (mainWindow);
  if (!isDebugMode)
    gtk_widget_hide (debugView);
  if (!needShowSideBar || !tvcontrolAsSidebar)
    gtk_widget_hide (sideView);
}

void
hide_infobar ()
{
  gtk_widget_destroy (infoBar);
  infoBar = NULL;
}

void
show_infobar (gchar *messageError)
{

  if (infoBar != NULL)
    hide_infobar ();

  infoBar = gtk_info_bar_new ();
  g_assert_nonnull (infoBar);
  GtkWidget *content_area
      = gtk_info_bar_get_content_area (GTK_INFO_BAR (infoBar));
  gtk_container_add (GTK_CONTAINER (content_area),
                     gtk_label_new (messageError));
  gtk_info_bar_set_show_close_button (GTK_INFO_BAR (infoBar), true);
  gtk_info_bar_set_message_type (GTK_INFO_BAR (infoBar), GTK_MESSAGE_ERROR);
  g_signal_connect (GTK_INFO_BAR (infoBar), "response",
                    G_CALLBACK (hide_infobar), NULL);

  gtk_box_pack_start (GTK_BOX (mainBox), infoBar, false, false, 0);
  gtk_box_reorder_child (GTK_BOX (mainBox), infoBar, 0);

  update_main_window ();
}

void
g_log_default_handler (unused (const gchar *log_domain),
                       GLogLevelFlags log_level, const gchar *message,
                       unused (gpointer unused_data))
{
  // printf ("%s \n", message);
  if (!g_str_has_prefix (message, "ginga::"))
    return; // is not a ginga message

  // insert message in console text view
  const char *time_label = "[00:00] ";
  gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (consoleTxtBuffer),
                                    time_label,
                                    g_utf8_strlen (time_label, -1));
  gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (consoleTxtBuffer),
                                    message, g_utf8_strlen (message, -1));
  gtk_text_buffer_insert_at_cursor (GTK_TEXT_BUFFER (consoleTxtBuffer),
                                    "\n", g_utf8_strlen ("\n", -1));

  if ((log_level == G_LOG_LEVEL_ERROR) || (log_level == G_LOG_FLAG_FATAL))
    {
      // send error log to server
      send_http_log_message (1, message);
      if (!strcmp (message,
                   "ginga::PlayerVideo::cb_Bus(): No decoder available "
                   "for type 'video/ogg'."))
        {
          show_infobar (
              (gchar *) "No decoder available for type 'video/ogg'.");
          GINGA->stop ();
        }
    }
  /*
  GtkTextIter start, end;

    GtkTextTag *tag = gtk_text_buffer_create_tag (
        consoleTxtBuffer, "blue_foreground", "foreground", "blue", NULL);
    gtk_text_buffer_get_iter_at_offset (consoleTxtBuffer, &start, 7);
    gtk_text_buffer_get_iter_at_offset (consoleTxtBuffer, &end, 12);
    gtk_text_buffer_apply_tag (consoleTxtBuffer, GTK_TEXT_TAG(tag), &start,
  &end);
  */
}

gboolean
resize_callback (GtkWidget *widget, GdkEventConfigure *e, gpointer data)
{
  (void) widget;
  (void) data;

  presentationAttributes.resolutionWidth = e->width;
  presentationAttributes.resolutionHeight = e->height;

  GINGA->resize (e->width, e->height);

  return FALSE;
}

void
press_aboutButton_callback ()
{
  create_about_window ();
}

void
press_helpButton_callback ()
{
  create_help_window ();
}

void
press_maximize_button_callback ()
{
  if (gtk_window_is_maximized (GTK_WINDOW (mainWindow)))
    gtk_window_unmaximize (GTK_WINDOW (mainWindow));
  else
    gtk_window_maximize (GTK_WINDOW (mainWindow));
}

void
press_minimize_button_callback ()
{
  gtk_window_iconify (GTK_WINDOW (mainWindow));
}

void
apply_theme ()
{
  GError *error = NULL;
  GtkCssProvider *css_provider = gtk_css_provider_get_default ();

  gchar *filename;
  if (presentationAttributes.guiTheme == 0)
    filename = g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "style",
                             "light.css", NULL);
  else
    filename = g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "style",
                             "dark.css", NULL);

  GFile *file = g_file_new_for_path (filename);
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

  GtkWidget *img_icon = gtk_image_new_from_file (g_build_path (
      G_DIR_SEPARATOR_S, GINGADATADIR, "icons", get_icon_folder (),
      (inPlayMode ? "pause-icon.png" : "play-icon.png"), NULL));
  gtk_button_set_image (GTK_BUTTON (playButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "stop-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (stopButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "fullscreen-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (fullscreenButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "settings-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (settingsButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "remote-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (remoteButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "debug-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (debugButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "openfile-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (openButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "history-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (histButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "tools-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (toolsButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "info-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (aboutButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "question-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (helpButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "question-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (helpButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "screen-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (bigpictureButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "bottomhide-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (consoleHideButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "rightride-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (sidebarHideButton), img_icon);

  img_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "window-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (sidebarWinButton), img_icon);

  gtk_widget_destroy (tvRemoteView);

  tvRemoteView = create_tvremote_buttons (10, 10);
  gtk_box_pack_start (GTK_BOX (tvcontrolBox), tvRemoteView, false, true, 0);

  update_main_window ();
}
void
aspect_combobox_changed (GtkComboBox *widget, unused (gpointer user_data))
{
  presentationAttributes.aspectRatio = gtk_combo_box_get_active (widget);

  save_settings ();
}

void
theme_combobox_changed (GtkComboBox *widget, unused (gpointer user_data))
{
  presentationAttributes.guiTheme = gtk_combo_box_get_active (widget);

  apply_theme ();

  save_settings ();
}

gchar *
get_icon_folder ()
{
  if (presentationAttributes.guiTheme == 0)
    return g_strdup ("light-theme");
  else
    return g_strdup ("dark-theme");
}

void
toggle_visual_debug_callback ()
{
  visualDebugEnabled = !visualDebugEnabled;
  GINGA->setOptionBool ("debug", visualDebugEnabled);
}

void
press_bigpicture_button_callback ()
{
  create_bigpicture_window ();
}

void
hide_sideview ()
{
  needShowSideBar = FALSE;
  gtk_widget_hide (sideView);
}

void
hide_debugview ()
{
  isDebugMode = FALSE;
  gtk_widget_hide (debugView);
}

void
hide_historicbox ()
{
#if GTK_CHECK_VERSION(3, 22, 0)
  gtk_popover_popdown (GTK_POPOVER (historicBoxPopOver));
#else
  gtk_widget_hide (historicBoxPopOver);
#endif
}

void
show_toolbox ()
{
#if GTK_CHECK_VERSION(3, 22, 0)
  gtk_popover_popup (GTK_POPOVER (toolBoxPopOver));
#else
  gtk_widget_show (toolBoxPopOver);
#endif
}

void
show_historicbox ()
{

  if (g_list_length (
          gtk_container_get_children (GTK_CONTAINER (historicBox)))
      == 0)
    return;

#if GTK_CHECK_VERSION(3, 22, 0)
  gtk_popover_popup (GTK_POPOVER (historicBoxPopOver));
#else
  gtk_widget_show (historicBoxPopOver);
#endif

#if GTK_CHECK_VERSION(3, 14, 0)
  GtkListBoxRow *row
      = gtk_list_box_get_selected_row (GTK_LIST_BOX (historicBox));
  gtk_list_box_unselect_row (GTK_LIST_BOX (historicBox), row);
#endif
}

void
show_optbox ()
{
#if GTK_CHECK_VERSION(3, 22, 0)
  gtk_popover_popup (GTK_POPOVER (optBoxPopOver));
#else
  gtk_widget_show (optBoxPopOver);
#endif
}

void
change_tvcontrol_to_window_mode ()
{
  gtk_widget_hide (sideView);
  tvcontrolAsSidebar = FALSE;
  needShowSideBar = FALSE;
  create_tvcontrol_window ();
}

void
insert_historicbox (gchar *filename)
{
  gtk_entry_set_text (GTK_ENTRY (fileEntry), filename);
  GList *childs = gtk_container_get_children (GTK_CONTAINER (historicBox));
  for (GList *l = childs; l != NULL; l = l->next)
    {
      GtkWidget *row = (GtkWidget *) l->data;
      GtkWidget *label = gtk_bin_get_child (GTK_BIN (row));
      if (strcmp (filename, gtk_label_get_text (GTK_LABEL (label))) == 0)
        {
          gtk_container_remove (GTK_CONTAINER (historicBox),
                                GTK_WIDGET (row));
        }
      if (l->next == NULL && g_list_length (childs) > 5)
        {
          gtk_container_remove (GTK_CONTAINER (historicBox),
                                GTK_WIDGET (row));
        }
    }
  GtkWidget *label = gtk_label_new (filename);
  gtk_widget_set_halign (label, GTK_ALIGN_START);

  gtk_list_box_prepend (GTK_LIST_BOX (historicBox), label);
  gtk_widget_show_all (historicBox);
}

void
select_historic_line (GtkListBox *box, GtkListBoxRow *row,
                      unused (gpointer user_data))
{
  GtkLabel *label = (GtkLabel *) gtk_bin_get_child (GTK_BIN (row));
  gchar *label_text = g_strdup (gtk_label_get_text (label));
  gtk_entry_set_text (GTK_ENTRY (fileEntry), gtk_label_get_text (label));
  gtk_container_remove (GTK_CONTAINER (box), GTK_WIDGET (row));

  gtk_list_box_insert (GTK_LIST_BOX (historicBox),
                       gtk_label_new (label_text), 0);

  gtk_widget_show_all (historicBox);
  hide_historicbox ();
}

void
keyboard_callback (unused (GtkWidget *widget), GdkEventKey *e,
                   gpointer type)
{
  const char *key = "\0";
  gboolean free_key = FALSE;

  switch (e->keyval)
    {
    case GDK_KEY_Escape: /* quit */
      if (isFullScreenMode)
        set_unfullscreen_mode ();
      else if (inBigPictureMode
               && (g_strcmp0 ((const char *) type, "press") != 0))
        destroy_bigpicture_window ();
      break;
    case GDK_KEY_Meta_L:
    case GDK_KEY_Meta_R:
    case GDK_KEY_Control_R:
    case GDK_KEY_Control_L:
      if (g_strcmp0 ((const char *) type, "press") == 0)
        isCrtlModifierActive = TRUE;
      else
        isCrtlModifierActive = FALSE;
      break;
    case GDK_KEY_F:
    case GDK_KEY_f:
      if (isCrtlModifierActive && !isFullScreenMode)
        set_fullscreen_mode ();
      break;
    case GDK_KEY_R:
    case GDK_KEY_r:
      if (isCrtlModifierActive)
        create_tvcontrol_window ();
      break;
    case GDK_KEY_B:
    case GDK_KEY_b:
      if (isCrtlModifierActive)
        create_bigpicture_window ();
      break;
    case GDK_KEY_D:
    case GDK_KEY_d:
      if (isCrtlModifierActive)
        enable_disable_debug ();
      break;
    case GDK_KEY_asterisk:
      key = "*";
      break;
    case GDK_KEY_numbersign:
      key = "#";
      break;
    case GDK_KEY_Return:
      if (inBigPictureMode)
        play_application_in_bigpicture ();
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
      if (inBigPictureMode
          && (g_strcmp0 ((const char *) type, "press") != 0))
        carrousel_rotate (1);
      break;
    case GDK_KEY_Right:
      key = "CURSOR_RIGHT";
      if (inBigPictureMode
          && (g_strcmp0 ((const char *) type, "press") != 0))
        carrousel_rotate (-1);
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

  if (GINGA->getState () == GINGA_STATE_PLAYING)
    GINGA->sendKey (std::string (key),
                    g_strcmp0 ((const char *) type, "press") == 0);
  /*  if (free_key)
      g_free (key); */
}

void
create_window_components ()
{

  GtkWidget *header_bar = gtk_header_bar_new ();
  g_assert_nonnull (header_bar);
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header_bar), true);
#ifdef G_OS_WIN32
  gtk_header_bar_set_decoration_layout (GTK_HEADER_BAR (header_bar),
                                        "menu:minimize,maximize,close");
#else
  gtk_header_bar_set_decoration_layout (GTK_HEADER_BAR (header_bar),
                                        "menu:maximize,close");
#endif
  /* begin hist box */
  if (historicBox == NULL)
    {
      historicBox = gtk_list_box_new ();
      g_assert_nonnull (historicBox);
    }

  fileEntry = gtk_entry_new ();
  g_assert_nonnull (fileEntry);
  gtk_widget_set_size_request (fileEntry, 400, -1);

  histButton = gtk_button_new ();
  g_assert_nonnull (histButton);
  gtk_widget_set_has_tooltip (histButton, true);
  gtk_widget_set_tooltip_text (histButton, "Historic");
  g_assert_nonnull (histButton);
  g_signal_connect (histButton, "clicked", G_CALLBACK (show_historicbox),
                    NULL);

  // populate_historic_box ();
  GtkListBoxRow *row_0
      = gtk_list_box_get_row_at_index (GTK_LIST_BOX (historicBox), 0);
  if (row_0 != NULL)
    {
      GtkWidget *label = gtk_bin_get_child (GTK_BIN (row_0));
      gtk_entry_set_text (GTK_ENTRY (fileEntry),
                          gtk_label_get_text (GTK_LABEL (label)));
    }

#if GTK_CHECK_VERSION(3, 14, 0)
  gtk_list_box_unselect_all (GTK_LIST_BOX (historicBox));
#endif
  g_signal_connect (historicBox, "row-activated",
                    G_CALLBACK (select_historic_line), NULL);
  gtk_list_box_set_activate_on_single_click (GTK_LIST_BOX (historicBox),
                                             true);

  historicBoxPopOver = gtk_popover_new (fileEntry);
  g_assert_nonnull (historicBoxPopOver);
  gtk_container_add (GTK_CONTAINER (historicBoxPopOver), historicBox);
  g_object_set (historicBox, "margin", 5, NULL);
  gtk_widget_show_all (historicBox);
  /* end hist box */

  openButton = gtk_button_new ();
  g_assert_nonnull (openButton);
  gtk_widget_set_has_tooltip (openButton, true);
  gtk_widget_set_tooltip_text (openButton, "Open file");
  g_signal_connect (openButton, "clicked",
                    G_CALLBACK (select_ncl_file_callback), NULL);

  playButton = gtk_button_new ();
  g_assert_nonnull (playButton);
  g_signal_connect (playButton, "clicked",
                    G_CALLBACK (play_pause_button_callback), NULL);

  stopButton = gtk_button_new ();
  g_assert_nonnull (stopButton);
  g_signal_connect (stopButton, "clicked",
                    G_CALLBACK (stop_button_callback), NULL);

  fullscreenButton = gtk_button_new ();
  g_assert_nonnull (fullscreenButton);
  gtk_widget_set_has_tooltip (fullscreenButton, true);
  gtk_widget_set_tooltip_text (fullscreenButton, "Fullscreen");
  g_signal_connect (fullscreenButton, "clicked",
                    G_CALLBACK (set_fullscreen_mode), NULL);

  volumeButton = gtk_volume_button_new ();
  g_assert_nonnull (volumeButton);

  settingsButton = gtk_button_new ();
  g_assert_nonnull (settingsButton);
  gtk_widget_set_has_tooltip (settingsButton, true);
  gtk_widget_set_tooltip_text (settingsButton, "Preferences");
  g_signal_connect (settingsButton, "clicked", G_CALLBACK (show_optbox),
                    NULL);

  remoteButton = gtk_button_new ();
  g_assert_nonnull (remoteButton);
  gtk_widget_set_has_tooltip (remoteButton, true);
  gtk_widget_set_tooltip_text (remoteButton, "Remote Control");
  g_signal_connect (remoteButton, "clicked",
                    G_CALLBACK (create_tvcontrol_window), NULL);

  toolsButton = gtk_button_new ();
  g_assert_nonnull (toolsButton);
  gtk_widget_set_has_tooltip (toolsButton, true);
  gtk_widget_set_tooltip_text (toolsButton, "Tools");
  g_signal_connect (toolsButton, "clicked", G_CALLBACK (show_toolbox),
                    NULL);

  debugButton = gtk_button_new ();
  g_assert_nonnull (debugButton);
  gtk_widget_set_has_tooltip (debugButton, true);
  gtk_widget_set_tooltip_text (debugButton, "Debug");
  g_signal_connect (debugButton, "clicked",
                    G_CALLBACK (enable_disable_debug), NULL);

  /* begin tool box */
  GtkWidget *tool_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  g_assert_nonnull (tool_box);

  gtk_box_pack_start (GTK_BOX (tool_box), remoteButton, false, false, 0);
  gtk_box_pack_start (GTK_BOX (tool_box), debugButton, false, false, 0);
  toolBoxPopOver = gtk_popover_new (toolsButton);
  g_assert_nonnull (toolBoxPopOver);

  gtk_container_add (GTK_CONTAINER (toolBoxPopOver), tool_box);
  g_object_set (tool_box, "margin", 5, NULL);
  gtk_widget_show_all (tool_box);
  /* end tool box */

  /* begin option box */
  GtkWidget *opt_video_frame = gtk_frame_new ("Video");
  g_assert_nonnull (opt_video_frame);
  gtk_frame_set_label_align (GTK_FRAME (opt_video_frame), 0, 1.0);

  GtkWidget *opt_gui_frame = gtk_frame_new ("GUI Theme");
  g_assert_nonnull (opt_gui_frame);

  GtkWidget *aspect_combobox = gtk_combo_box_text_new ();
  g_assert_nonnull (aspect_combobox);
  g_object_set (aspect_combobox, "margin", 5, NULL);

  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "TV (4:3)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "HDTV (16:9)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "LCD (16:10)");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (aspect_combobox), -1,
                                  "Expanded");
  gtk_combo_box_set_active (GTK_COMBO_BOX (aspect_combobox),
                            presentationAttributes.aspectRatio);
  g_signal_connect (aspect_combobox, "changed",
                    G_CALLBACK (aspect_combobox_changed), NULL);

  GtkWidget *theme_combobox = gtk_combo_box_text_new ();
  g_assert_nonnull (theme_combobox);
  g_object_set (theme_combobox, "margin", 5, NULL);

  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (theme_combobox), -1,
                                  "Light");
  gtk_combo_box_text_insert_text (GTK_COMBO_BOX_TEXT (theme_combobox), -1,
                                  "Dark");
  gtk_combo_box_set_active (GTK_COMBO_BOX (theme_combobox),
                            presentationAttributes.guiTheme);

  g_signal_connect (theme_combobox, "changed",
                    G_CALLBACK (theme_combobox_changed), NULL);

  GtkWidget *opt_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  g_assert_nonnull (opt_box);

  gtk_box_pack_start (GTK_BOX (opt_box), gtk_label_new ("Aspect Ratio:"),
                      false, false, 0);
  gtk_box_pack_start (GTK_BOX (opt_box), aspect_combobox, false, true, 0);

  aboutButton = gtk_button_new ();
  g_assert_nonnull (aboutButton);
  gtk_widget_set_has_tooltip (aboutButton, true);
  gtk_widget_set_tooltip_text (aboutButton, "About");
  g_signal_connect (aboutButton, "clicked",
                    G_CALLBACK (press_aboutButton_callback), NULL);

  helpButton = gtk_button_new ();
  g_assert_nonnull (helpButton);
  gtk_widget_set_has_tooltip (helpButton, true);
  gtk_widget_set_tooltip_text (helpButton, "Help");
  g_signal_connect (helpButton, "clicked",
                    G_CALLBACK (press_helpButton_callback), NULL);

  bigpictureButton = gtk_button_new ();
  g_assert_nonnull (bigpictureButton);
  gtk_widget_set_has_tooltip (bigpictureButton, true);
  gtk_widget_set_tooltip_text (bigpictureButton, "Presentation Mode");
  g_signal_connect (bigpictureButton, "clicked",
                    G_CALLBACK (press_bigpicture_button_callback), NULL);
  gtk_widget_set_sensitive (bigpictureButton, false);

  GtkWidget *info_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  g_assert_nonnull (info_box);
  gtk_box_set_homogeneous (GTK_BOX (info_box), true);

  gtk_box_pack_start (GTK_BOX (info_box), aboutButton, false, false, 0);
  gtk_box_pack_start (GTK_BOX (info_box), helpButton, false, false, 0);
  gtk_box_pack_start (GTK_BOX (info_box), bigpictureButton, false, false,
                      0);

  optBoxPopOver = gtk_popover_new (settingsButton);
  g_assert_nonnull (optBoxPopOver);

  gtk_container_add (GTK_CONTAINER (opt_video_frame), opt_box);
  gtk_container_add (GTK_CONTAINER (opt_gui_frame), theme_combobox);

  GtkWidget *opt_hbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);
  g_assert_nonnull (opt_hbox);

  gtk_box_pack_start (GTK_BOX (opt_hbox), opt_video_frame, false, false, 0);
  gtk_box_pack_start (GTK_BOX (opt_hbox), opt_gui_frame, false, false, 0);
  gtk_box_pack_start (GTK_BOX (opt_hbox), info_box, false, false, 0);

  gtk_container_add (GTK_CONTAINER (optBoxPopOver), opt_hbox);
  g_object_set (opt_hbox, "margin", 5, NULL);
  gtk_widget_show_all (opt_hbox);

  /* end option box */

  GtkWidget *sep2 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep2, 1, 30);

  GtkWidget *sep3 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep3, 1, 30);

  GtkWidget *sep4 = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
  gtk_widget_set_size_request (sep3, 1, 30);

  GtkWidget *file_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);

  gtk_box_pack_start (GTK_BOX (file_box), openButton, false, false, 0);
  gtk_box_pack_start (GTK_BOX (file_box), fileEntry, false, false, 0);
  gtk_box_pack_start (GTK_BOX (file_box), histButton, false, false, 0);

  gtk_header_bar_set_custom_title (GTK_HEADER_BAR (header_bar), file_box);

  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar), settingsButton);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar), fullscreenButton);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (header_bar), sep2);

  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), playButton);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), stopButton);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), sep3);
  //  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), volumeButton);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), toolsButton);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), sep4);
  gtk_window_set_titlebar (GTK_WINDOW (mainWindow), header_bar);

  // Create Drawing area
  gingaView = gtk_drawing_area_new ();
  g_assert_nonnull (gingaView);
  gtk_widget_set_app_paintable (gingaView, TRUE);

  g_signal_connect (gingaView, "draw", G_CALLBACK (draw_callback), NULL);
  gtk_widget_set_size_request (gingaView,
                               presentationAttributes.resolutionWidth,
                               presentationAttributes.resolutionHeight);

#if GTK_CHECK_VERSION(3, 8, 0)
  gtk_widget_add_tick_callback (
      gingaView, (GtkTickCallback) update_draw_callback, NULL, NULL);
#else
  g_timeout_add (1000 / 60, (GSourceFunc) update_draw_callback, gingaView);
#endif

  /* begin debug view   */

  GtkWidget *debugTextArea = gtk_text_view_new ();
  g_assert_nonnull (debugTextArea);
  consoleTxtBuffer
      = gtk_text_view_get_buffer (GTK_TEXT_VIEW (debugTextArea));
  gtk_text_view_set_editable (GTK_TEXT_VIEW (debugTextArea), false);
  GtkWidget *scrolledWLog = gtk_scrolled_window_new (NULL, NULL);
  g_assert_nonnull (scrolledWLog);
  gtk_container_add (GTK_CONTAINER (scrolledWLog), debugTextArea);

  debugView = gtk_notebook_new ();
  g_assert_nonnull (debugView);
  gtk_widget_set_size_request (debugView,
                               presentationAttributes.resolutionWidth, 140);
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (debugView), false);
  gtk_widget_set_margin_top (debugView, 5);

  visualDebugSwitch = gtk_check_button_new_with_label ("Visual Debug   ");
  g_assert_nonnull (visualDebugSwitch);
  g_signal_connect (visualDebugSwitch, "clicked",
                    G_CALLBACK (toggle_visual_debug_callback), NULL);

  consoleHideButton = gtk_button_new ();
  g_assert_nonnull (consoleHideButton);
  g_signal_connect (consoleHideButton, "clicked",
                    G_CALLBACK (hide_debugview), NULL);

  GtkWidget *debug_headbar = gtk_header_bar_new ();
  g_assert_nonnull (debug_headbar);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (debug_headbar),
                             gtk_label_new ("  Console"));
  gtk_header_bar_pack_end (GTK_HEADER_BAR (debug_headbar),
                           consoleHideButton);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (debug_headbar),
                           visualDebugSwitch);
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

  sidebarWinButton = gtk_button_new ();
  g_assert_nonnull (sidebarWinButton);
  g_signal_connect (sidebarWinButton, "clicked",
                    G_CALLBACK (change_tvcontrol_to_window_mode), NULL);

  sidebarHideButton = gtk_button_new ();
  g_assert_nonnull (sidebarHideButton);
  g_signal_connect (sidebarHideButton, "clicked",
                    G_CALLBACK (hide_sideview), NULL);

  GtkWidget *tvcontrol_headbar = gtk_header_bar_new ();
  g_assert_nonnull (tvcontrol_headbar);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (tvcontrol_headbar),
                             sidebarWinButton);
  gtk_header_bar_pack_end (GTK_HEADER_BAR (tvcontrol_headbar),
                           sidebarHideButton);
  gtk_header_bar_set_title (GTK_HEADER_BAR (tvcontrol_headbar), "Control");
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (tvcontrol_headbar),
                                        false);

  tvcontrolBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  g_assert_nonnull (tvcontrolBox);

  gtk_box_pack_start (GTK_BOX (tvcontrolBox), tvcontrol_headbar, false,
                      true, 0);

  tvRemoteView = create_tvremote_buttons (10, 10);
  gtk_box_pack_start (GTK_BOX (tvcontrolBox), tvRemoteView, false, true, 0);

  gtk_notebook_append_page (GTK_NOTEBOOK (sideView), tvcontrolBox,
                            gtk_label_new ("Control"));

  GtkWidget *h_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  g_assert_nonnull (h_box);

  mainBox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  g_assert_nonnull (mainBox);

  // gtk_box_pack_start (GTK_BOX (mainBox), infoBar, false, true, 0);
  gtk_box_pack_start (GTK_BOX (mainBox), gingaView, true, true, 0);

  gtk_box_pack_start (GTK_BOX (h_box), mainBox, true, true, 0);
  gtk_box_pack_start (GTK_BOX (h_box), sideView, false, true, 0);

  /* end side view */

  GtkWidget *vpaned = gtk_paned_new (GTK_ORIENTATION_VERTICAL);
  g_assert_nonnull (vpaned);
  // gtk_widget_set_size_request (vpaned, 200, -1);

  gtk_paned_pack1 (GTK_PANED (vpaned), h_box, true, true);
  gtk_paned_pack2 (GTK_PANED (vpaned), debugView, true, false);

  gtk_container_add (GTK_CONTAINER (mainWindow), vpaned);

  apply_theme ();

  show_tracker_dialog (GTK_WINDOW (mainWindow));
}

void
create_main_window ()
{
  g_log_set_default_handler (g_log_default_handler, NULL);

  // Create application window.
  mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (mainWindow);
  gtk_window_set_title (GTK_WINDOW (mainWindow), "Ginga");
  gtk_window_set_default_size (GTK_WINDOW (mainWindow),
                               presentationAttributes.resolutionWidth,
                               presentationAttributes.resolutionHeight);
  gtk_window_set_position (GTK_WINDOW (mainWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (mainWindow), true);
  gtk_container_set_border_width (GTK_CONTAINER (mainWindow), 5);

  create_window_components ();

  g_signal_connect (mainWindow, "key-press-event",
                    G_CALLBACK (keyboard_callback), (void *) "press");
  g_signal_connect (mainWindow, "key-release-event",
                    G_CALLBACK (keyboard_callback), (void *) "release");
  g_signal_connect (mainWindow, "destroy", G_CALLBACK (destroy_main_window),
                    NULL);
  // g_signal_connect (window, "delete-event", G_CALLBACK
  // (gtk_widget_hide_on_delete), NULL);

  g_signal_connect (gingaView, "configure-event",
                    G_CALLBACK (resize_callback), NULL);

  gtk_widget_show_all (mainWindow);

  gtk_widget_hide (debugView);
  gtk_widget_hide (sideView);
  gtk_widget_hide (infoBar);
}

void
destroy_main_window (void)
{
  if (GINGA->getState () != GINGA_STATE_STOPPED)
    GINGA->stop ();

  gtk_widget_destroy (mainWindow);
  mainWindow = NULL;

  gtk_main_quit ();
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

void select_ncl_file_callback (unused (GtkWidget *widget),
                               unused (gpointer data))
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
          show_infobar (g_markup_printf_escaped (
              "Error reading \"%s\": %s is a invalid formart file.",
              filename, ext));
        }
      else
        {
          insert_historicbox (filename);
          save_settings ();
        }

      g_free (filename);
    }

  gtk_widget_destroy (dialog);
}

void
stop_button_callback (void)
{
  inPlayMode = false;
  GtkWidget *play_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "play-icon.png", NULL));
  gtk_button_set_image (GTK_BUTTON (playButton), play_icon);
  gtk_widget_set_sensitive (fileEntry, true);
  gtk_widget_set_sensitive (openButton, true);
  gtk_widget_set_sensitive (histButton, true);

  GINGA->stop ();
}

void
play_pause_button_callback (void)
{
  inPlayMode = !inPlayMode;
  GtkWidget *play_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "play-icon.png", NULL));
  if (inPlayMode)
    {
      const gchar *file = gtk_entry_get_text (GTK_ENTRY (fileEntry));
      const gchar *ext = strrchr (file, '.');
      if (g_strcmp0 (ext, ".ncl"))
        {
          show_infobar (g_markup_printf_escaped (
              "Error reading \"%s\": %s is a invalid formart file.", file,
              ext));
          return;
        }

      play_icon = gtk_image_new_from_file (
          g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                        get_icon_folder (), "pause-icon.png", NULL));
      gtk_widget_set_sensitive (fileEntry, false);
      gtk_widget_set_sensitive (openButton, false);
      gtk_widget_set_sensitive (histButton, false);

      // Start Ginga.
      GINGA->start (file, nullptr);

      // clear log
      gtk_text_buffer_set_text (consoleTxtBuffer, "", 0);
    }

  gtk_button_set_image (GTK_BUTTON (playButton), play_icon);
}
