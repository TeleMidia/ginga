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

GtkWidget *tvcontrolWindow = NULL;
gboolean tvcontrolAsSidebar = TRUE;

void
key_tvremote_press_event_callback (GtkWidget *widget,
                                   unused (gpointer data))
{
  const gchar *widget_name = gtk_widget_get_name (widget);
  GINGA->sendKey (std::string (widget_name), true);
}

GtkWidget *
create_tvremote_buttons (guint offSetX, guint offSetY)
{
  guint16 control_width = (BUTTON_SIZE * 4);
  guint16 middle_button_pos = (control_width / 2) - (BUTTON_SIZE / 2);

  GtkWidget *fixed_layout = gtk_fixed_new ();
  g_assert_nonnull (fixed_layout);

  GtkWidget *button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_one.png", NULL));
  GtkWidget *button_1 = gtk_button_new ();
  g_assert_nonnull (button_1);
  gtk_button_set_image (GTK_BUTTON (button_1), button_icon);
  gtk_widget_set_name (button_1, "1");
  g_signal_connect (button_1, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_1,
                 offSetX + middle_button_pos - BUTTON_SIZE, offSetY + 0);

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_two.png", NULL));
  GtkWidget *button_2 = gtk_button_new ();
  g_assert_nonnull (button_2);
  gtk_button_set_image (GTK_BUTTON (button_2), button_icon);
  gtk_widget_set_name (button_2, "2");
  g_signal_connect (button_2, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_2,
                 offSetX + middle_button_pos, offSetY + 0);

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_three.png", NULL));
  GtkWidget *button_3 = gtk_button_new ();
  g_assert_nonnull (button_3);
  gtk_button_set_image (GTK_BUTTON (button_3), button_icon);
  gtk_widget_set_name (button_3, "3");
  g_signal_connect (button_3, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_3,
                 offSetX + middle_button_pos + BUTTON_SIZE, offSetY + 0);

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_four.png", NULL));
  GtkWidget *button_4 = gtk_button_new ();
  g_assert_nonnull (button_4);
  gtk_button_set_image (GTK_BUTTON (button_4), button_icon);
  gtk_widget_set_name (button_4, "4");
  g_signal_connect (button_4, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_4,
                 offSetX + middle_button_pos - BUTTON_SIZE,
                 offSetY + BUTTON_SIZE);

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_five.png", NULL));
  GtkWidget *button_5 = gtk_button_new ();
  g_assert_nonnull (button_5);
  gtk_button_set_image (GTK_BUTTON (button_5), button_icon);
  gtk_widget_set_name (button_5, "5");
  g_signal_connect (button_5, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_5,
                 offSetX + middle_button_pos, offSetY + BUTTON_SIZE);

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_six.png", NULL));
  GtkWidget *button_6 = gtk_button_new ();
  g_assert_nonnull (button_6);
  gtk_button_set_image (GTK_BUTTON (button_6), button_icon);
  gtk_widget_set_name (button_6, "6");
  g_signal_connect (button_6, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_6,
                 offSetX + middle_button_pos + BUTTON_SIZE,
                 offSetY + BUTTON_SIZE);

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_seven.png", NULL));
  GtkWidget *button_7 = gtk_button_new ();
  g_assert_nonnull (button_7);
  gtk_button_set_image (GTK_BUTTON (button_7), button_icon);
  gtk_widget_set_name (button_7, "7");
  g_signal_connect (button_7, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_7,
                 offSetX + middle_button_pos - BUTTON_SIZE,
                 offSetY + (BUTTON_SIZE * 2));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_eight.png", NULL));
  GtkWidget *button_8 = gtk_button_new ();
  g_assert_nonnull (button_8);
  gtk_button_set_image (GTK_BUTTON (button_8), button_icon);
  gtk_widget_set_name (button_8, "8");
  g_signal_connect (button_8, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_8,
                 offSetX + middle_button_pos, offSetY + (BUTTON_SIZE * 2));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_nine.png", NULL));
  GtkWidget *button_9 = gtk_button_new ();
  g_assert_nonnull (button_9);
  gtk_button_set_image (GTK_BUTTON (button_9), button_icon);
  gtk_widget_set_name (button_9, "9");
  g_signal_connect (button_9, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_9,
                 offSetX + middle_button_pos + BUTTON_SIZE,
                 offSetY + (BUTTON_SIZE * 2));

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "back_arrow.png", NULL));
  GtkWidget *button_back = gtk_button_new ();
  g_assert_nonnull (button_back);
  gtk_button_set_image (GTK_BUTTON (button_back), button_icon);
  gtk_widget_set_name (button_back, "RETURN");
  g_signal_connect (button_back, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_back,
                 offSetX + middle_button_pos - BUTTON_SIZE,
                 offSetY + (BUTTON_SIZE * 3));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "number_zero.png", NULL));
  GtkWidget *button_0 = gtk_button_new ();
  g_assert_nonnull (button_0);
  gtk_button_set_image (GTK_BUTTON (button_0), button_icon);
  gtk_widget_set_name (button_0, "0");
  g_signal_connect (button_0, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_0,
                 offSetX + middle_button_pos, offSetY + (BUTTON_SIZE * 3));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "info-icon.png", NULL));
  GtkWidget *button_info = gtk_button_new ();
  g_assert_nonnull (button_info);
  gtk_button_set_image (GTK_BUTTON (button_info), button_icon);
  gtk_widget_set_name (button_info, "INFO");
  g_signal_connect (button_info, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_info,
                 offSetX + middle_button_pos + BUTTON_SIZE,
                 offSetY + (BUTTON_SIZE * 3));

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "uparrow-icon.png", NULL));
  GtkWidget *button_up = gtk_button_new ();
  g_assert_nonnull (button_up);
  gtk_button_set_image (GTK_BUTTON (button_up), button_icon);
  gtk_widget_set_name (button_up, "CURSOR_UP");
  g_signal_connect (button_up, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_up,
                 offSetX + middle_button_pos,
                 offSetY + 10 + (BUTTON_SIZE * 4));

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "leftarrow-icon.png", NULL));
  GtkWidget *button_left = gtk_button_new ();
  g_assert_nonnull (button_left);
  gtk_button_set_image (GTK_BUTTON (button_left), button_icon);
  gtk_widget_set_name (button_left, "CURSOR_LEFT");
  g_signal_connect (button_left, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_left,
                 offSetX + middle_button_pos - BUTTON_SIZE,
                 offSetY + 10 + (BUTTON_SIZE * 5));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "ok-icon.png", NULL));
  GtkWidget *button_ok = gtk_button_new ();
  g_assert_nonnull (button_ok);
  gtk_button_set_image (GTK_BUTTON (button_ok), button_icon);
  gtk_widget_set_name (button_ok, "OK");
  g_signal_connect (button_ok, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_ok,
                 offSetX + middle_button_pos,
                 offSetY + 10 + (BUTTON_SIZE * 5));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "rightarrow-icon.png", NULL));
  GtkWidget *button_right = gtk_button_new ();
  g_assert_nonnull (button_right);
  gtk_button_set_image (GTK_BUTTON (button_right), button_icon);
  gtk_widget_set_name (button_right, "CURSOR_RIGHT");
  g_signal_connect (button_right, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_right,
                 offSetX + middle_button_pos + BUTTON_SIZE,
                 offSetY + 10 + (BUTTON_SIZE * 5));

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "downarrow-icon.png", NULL));
  GtkWidget *button_down = gtk_button_new ();
  g_assert_nonnull (button_down);
  gtk_button_set_image (GTK_BUTTON (button_down), button_icon);
  gtk_widget_set_name (button_down, "CURSOR_DOWN");
  g_signal_connect (button_down, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_down,
                 offSetX + middle_button_pos,
                 offSetY + 10 + (BUTTON_SIZE * 6));

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "volup-icon.png", NULL));
  GtkWidget *button_vol_up = gtk_button_new ();
  g_assert_nonnull (button_vol_up);
  gtk_button_set_image (GTK_BUTTON (button_vol_up), button_icon);
  gtk_widget_set_name (button_vol_up, "b_vol_up");
  g_signal_connect (button_vol_up, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_vol_up, offSetX + 0,
                 offSetY + 20 + (BUTTON_SIZE * 7));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "menu-icon.png", NULL));
  GtkWidget *button_menu = gtk_button_new ();
  g_assert_nonnull (button_menu);
  gtk_button_set_image (GTK_BUTTON (button_menu), button_icon);
  gtk_widget_set_name (button_menu, "b_menu");
  g_signal_connect (button_menu, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_menu,
                 offSetX + BUTTON_SIZE, offSetY + 20 + (BUTTON_SIZE * 7));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "close-icon.png", NULL));
  GtkWidget *button_close = gtk_button_new ();
  g_assert_nonnull (button_close);
  gtk_button_set_image (GTK_BUTTON (button_close), button_icon);
  gtk_widget_set_name (button_close, "b_close");
  g_signal_connect (button_close, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_close,
                 offSetX + (BUTTON_SIZE * 2),
                 offSetY + 20 + (BUTTON_SIZE * 7));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "chup-icon.png", NULL));
  GtkWidget *button_ch_up = gtk_button_new ();
  g_assert_nonnull (button_ch_up);
  gtk_button_set_image (GTK_BUTTON (button_ch_up), button_icon);
  gtk_widget_set_name (button_ch_up, "b_ch_up");
  g_signal_connect (button_ch_up, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_ch_up,
                 offSetX + (BUTTON_SIZE * 3),
                 offSetY + 20 + (BUTTON_SIZE * 7));

  //

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "voldown-icon.png", NULL));
  GtkWidget *button_vol_down = gtk_button_new ();
  g_assert_nonnull (button_vol_down);
  gtk_button_set_image (GTK_BUTTON (button_vol_down), button_icon);
  gtk_widget_set_name (button_vol_down, "b_vol_down");
  g_signal_connect (button_vol_down, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_vol_down, offSetX + 0,
                 offSetY + 20 + (BUTTON_SIZE * 8));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "settings-icon.png", NULL));
  GtkWidget *button_menu2 = gtk_button_new ();
  g_assert_nonnull (button_menu2);
  gtk_button_set_image (GTK_BUTTON (button_menu2), button_icon);
  gtk_widget_set_name (button_menu2, "b_menu2");
  g_signal_connect (button_menu2, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_menu2,
                 offSetX + BUTTON_SIZE, offSetY + 20 + (BUTTON_SIZE * 8));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "settings-icon.png", NULL));
  GtkWidget *button_info2 = gtk_button_new ();
  g_assert_nonnull (button_info2);
  gtk_button_set_image (GTK_BUTTON (button_info2), button_icon);
  gtk_widget_set_name (button_info2, "b_info2");
  g_signal_connect (button_info2, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_info2,
                 offSetX + (BUTTON_SIZE * 2),
                 offSetY + 20 + (BUTTON_SIZE * 8));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "chdown-icon.png", NULL));
  GtkWidget *button_ch_down = gtk_button_new ();
  g_assert_nonnull (button_ch_down);
  gtk_button_set_image (GTK_BUTTON (button_ch_down), button_icon);
  gtk_widget_set_name (button_ch_down, "b_ch_down");
  g_signal_connect (button_ch_down, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_ch_down,
                 offSetX + (BUTTON_SIZE * 3),
                 offSetY + 20 + (BUTTON_SIZE * 8));

  //

  button_icon = gtk_image_new_from_file (g_build_path (
      G_DIR_SEPARATOR_S, GINGADATADIR, "icons/common/red-icon.png", NULL));
  GtkWidget *button_red = gtk_button_new ();
  g_assert_nonnull (button_red);
  gtk_button_set_image (GTK_BUTTON (button_red), button_icon);
  gtk_widget_set_name (button_red, "RED");
  g_signal_connect (button_red, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_red, offSetX + 0,
                 offSetY + 20 + (BUTTON_SIZE * 9));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR,
                    "icons/common/green-icon.png", NULL));
  GtkWidget *button_green = gtk_button_new ();
  g_assert_nonnull (button_green);
  gtk_button_set_image (GTK_BUTTON (button_green), button_icon);
  gtk_widget_set_name (button_green, "GREEN");
  g_signal_connect (button_green, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_green,
                 offSetX + BUTTON_SIZE, offSetY + 20 + (BUTTON_SIZE * 9));

  button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR,
                    "icons/common/yellow-icon.png", NULL));
  GtkWidget *button_yellow = gtk_button_new ();
  g_assert_nonnull (button_yellow);
  gtk_button_set_image (GTK_BUTTON (button_yellow), button_icon);
  gtk_widget_set_name (button_yellow, "YELLOW");
  g_signal_connect (button_yellow, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_yellow,
                 offSetX + (BUTTON_SIZE * 2),
                 offSetY + 20 + (BUTTON_SIZE * 9));

  button_icon = gtk_image_new_from_file (g_build_path (
      G_DIR_SEPARATOR_S, GINGADATADIR, "icons/common/blue-icon.png", NULL));
  GtkWidget *button_blue = gtk_button_new ();
  g_assert_nonnull (button_blue);
  gtk_button_set_image (GTK_BUTTON (button_blue), button_icon);
  gtk_widget_set_name (button_blue, "BLUE");
  g_signal_connect (button_blue, "clicked",
                    G_CALLBACK (key_tvremote_press_event_callback), NULL);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_blue,
                 offSetX + (BUTTON_SIZE * 3),
                 offSetY + 20 + (BUTTON_SIZE * 9));

  GtkWidget *label = gtk_label_new ("...");
  g_assert_nonnull (label);
  gtk_fixed_put (GTK_FIXED (fixed_layout), label,
                 offSetX + middle_button_pos + 10,
                 offSetY + 20 + (BUTTON_SIZE * 10));

  return fixed_layout;
}

void
show_tvremote_sidebar ()
{
  tvcontrolAsSidebar = TRUE;
  needShowSideBar = TRUE;

  if (tvcontrolWindow != NULL)
    destroy_tvcontrol_window ();
  gtk_widget_show_all (mainWindow);
  if (!isDebugMode)
    gtk_widget_hide (debugView);
  if (!needShowErrorBar)
    gtk_widget_hide (infoBar);
}

void
create_tvcontrol_window (void)
{

  if (tvcontrolAsSidebar)
    {
      needShowSideBar = !needShowSideBar;
      if (needShowSideBar)
        show_tvremote_sidebar ();
      else
        gtk_widget_hide (sideView);
      return;
    }

  if (tvcontrolWindow != NULL)
    return;

  guint16 control_width = (BUTTON_SIZE * 4);
  guint16 control_height = (BUTTON_SIZE * 11);

  GtkWidget *header_bar = gtk_header_bar_new ();
  g_assert_nonnull (header_bar);
  gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (header_bar), true);
  gtk_header_bar_set_decoration_layout (GTK_HEADER_BAR (header_bar),
                                        "menu:minimize,maximize,close");

  tvcontrolWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (tvcontrolWindow);
  gtk_window_set_title (GTK_WINDOW (tvcontrolWindow), "Control");
  gtk_window_set_default_size (GTK_WINDOW (tvcontrolWindow), control_width,
                               control_height);
  gtk_window_set_position (GTK_WINDOW (tvcontrolWindow),
                           GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (tvcontrolWindow), FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (tvcontrolWindow), 5);
  gtk_window_set_titlebar (GTK_WINDOW (tvcontrolWindow), header_bar);
  gtk_window_set_type_hint (GTK_WINDOW (tvcontrolWindow),
                            GDK_WINDOW_TYPE_HINT_DIALOG);
  gtk_header_bar_set_title (GTK_HEADER_BAR (header_bar), "Control");

  GtkWidget *button_icon = gtk_image_new_from_file (
      g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR, "icons",
                    get_icon_folder (), "sidebar-icon.png", NULL));
  g_assert_nonnull (button_icon);

  GtkWidget *sidebar_button = gtk_button_new ();
  g_assert_nonnull (sidebar_button);
  gtk_button_set_image (GTK_BUTTON (sidebar_button), button_icon);
  g_signal_connect (sidebar_button, "clicked",
                    G_CALLBACK (show_tvremote_sidebar), NULL);
  gtk_header_bar_pack_start (GTK_HEADER_BAR (header_bar), sidebar_button);

  GtkWidget *layout = create_tvremote_buttons (0, 0);

  gtk_container_add (GTK_CONTAINER (tvcontrolWindow), layout);

  g_signal_connect (tvcontrolWindow, "destroy",
                    G_CALLBACK (destroy_tvcontrol_window), NULL);

  gtk_widget_show_all (tvcontrolWindow);
}

void
destroy_tvcontrol_window (void)
{
  gtk_widget_destroy (tvcontrolWindow);
  tvcontrolWindow = NULL;
}
