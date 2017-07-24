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

GtkWidget *tvcontrolWindow = NULL;

void
create_tvcontrol_window (void)
{

  if (tvcontrolWindow != NULL)
    return;

  guint16 control_width = (BUTTON_SIZE * 4);
  guint16 control_height = (BUTTON_SIZE * 11);
  guint16 middle_button_pos = (control_width / 2) - (BUTTON_SIZE / 2);

  tvcontrolWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (tvcontrolWindow);
  gtk_window_set_title (GTK_WINDOW (tvcontrolWindow), "Control");
  gtk_window_set_default_size (GTK_WINDOW (tvcontrolWindow), control_width,
                               control_height);
  gtk_window_set_position (GTK_WINDOW (tvcontrolWindow),
                           GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (tvcontrolWindow), FALSE);
  gtk_container_set_border_width (GTK_CONTAINER (tvcontrolWindow),
                                  ginga_gui.default_margin);

  GtkWidget *fixed_layout = gtk_fixed_new ();
  g_assert_nonnull (fixed_layout);
  
  GtkWidget *button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_one.png", NULL));
  GtkWidget *button_1 = gtk_button_new ();
  g_assert_nonnull (button_1);
  gtk_button_set_image (GTK_BUTTON (button_1), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_1,
                 middle_button_pos - BUTTON_SIZE, 0);

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_two.png", NULL));
  GtkWidget *button_2 = gtk_button_new ();
  g_assert_nonnull (button_2);
  gtk_button_set_image (GTK_BUTTON (button_2), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_2, middle_button_pos, 0);

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_three.png", NULL));
  GtkWidget *button_3 = gtk_button_new ();
  g_assert_nonnull (button_3);
  gtk_button_set_image (GTK_BUTTON (button_3), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_3,
                 middle_button_pos + BUTTON_SIZE, 0);

  //

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_four.png", NULL));
  GtkWidget *button_4 = gtk_button_new ();
  g_assert_nonnull (button_4);
  gtk_button_set_image (GTK_BUTTON (button_4), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_4,
                 middle_button_pos - BUTTON_SIZE, BUTTON_SIZE);

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_five.png", NULL));
  GtkWidget *button_5 = gtk_button_new ();
  g_assert_nonnull (button_5);
  gtk_button_set_image (GTK_BUTTON (button_5), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_5, middle_button_pos,
                 BUTTON_SIZE);

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_six.png", NULL));
  GtkWidget *button_6 = gtk_button_new ();
  g_assert_nonnull (button_6);
  gtk_button_set_image (GTK_BUTTON (button_6), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_6,
                 middle_button_pos + BUTTON_SIZE, BUTTON_SIZE);

  //

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_seven.png", NULL));
  GtkWidget *button_7 = gtk_button_new ();
  g_assert_nonnull (button_7);
  gtk_button_set_image (GTK_BUTTON (button_7), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_7,
                 middle_button_pos - BUTTON_SIZE, (BUTTON_SIZE * 2));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_eight.png", NULL));
  GtkWidget *button_8 = gtk_button_new ();
  g_assert_nonnull (button_8);
  gtk_button_set_image (GTK_BUTTON (button_8), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_8, middle_button_pos,
                 (BUTTON_SIZE * 2));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_nine.png", NULL));
  GtkWidget *button_9 = gtk_button_new ();
  g_assert_nonnull (button_9);
  gtk_button_set_image (GTK_BUTTON (button_9), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_9,
                 middle_button_pos + BUTTON_SIZE, (BUTTON_SIZE * 2));

  //

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/back_arrow.png", NULL));
  GtkWidget *button_back = gtk_button_new ();
  g_assert_nonnull (button_back);
  gtk_button_set_image (GTK_BUTTON (button_back), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_back,
                 middle_button_pos - BUTTON_SIZE, (BUTTON_SIZE * 3));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/number_zero.png", NULL));
  GtkWidget *button_0 = gtk_button_new ();
  g_assert_nonnull (button_0);
  gtk_button_set_image (GTK_BUTTON (button_0), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_0, middle_button_pos,
                 (BUTTON_SIZE * 3));

  button_icon = gtk_image_new_from_icon_name ("dialog-information",
                                              GTK_ICON_SIZE_BUTTON);
  GtkWidget *button_info = gtk_button_new ();
  g_assert_nonnull (button_info);
  gtk_button_set_image (GTK_BUTTON (button_info), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_info,
                 middle_button_pos + BUTTON_SIZE, (BUTTON_SIZE * 3));

  //

  button_icon
      = gtk_image_new_from_icon_name ("go-top", GTK_ICON_SIZE_BUTTON);
  GtkWidget *button_up = gtk_button_new ();
  g_assert_nonnull (button_up);
  gtk_button_set_image (GTK_BUTTON (button_up), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_up, middle_button_pos,
                 10 + (BUTTON_SIZE * 4));

  //

  button_icon
      = gtk_image_new_from_icon_name ("go-first", GTK_ICON_SIZE_BUTTON);
  GtkWidget *button_left = gtk_button_new ();
  g_assert_nonnull (button_left);
  gtk_button_set_image (GTK_BUTTON (button_left), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_left,
                 middle_button_pos - BUTTON_SIZE, 10 + (BUTTON_SIZE * 5));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/ok_button.png", NULL));
  GtkWidget *button_ok = gtk_button_new ();
  g_assert_nonnull (button_ok);
  gtk_button_set_image (GTK_BUTTON (button_ok), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_ok, middle_button_pos,
                 10 + (BUTTON_SIZE * 5));

  button_icon
      = gtk_image_new_from_icon_name ("go-last", GTK_ICON_SIZE_BUTTON);
  GtkWidget *button_right = gtk_button_new ();
  g_assert_nonnull (button_right);
  gtk_button_set_image (GTK_BUTTON (button_right), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_right,
                 middle_button_pos + BUTTON_SIZE, 10 + (BUTTON_SIZE * 5));

  //

  button_icon
      = gtk_image_new_from_icon_name ("go-bottom", GTK_ICON_SIZE_BUTTON);
  GtkWidget *button_down = gtk_button_new ();
  g_assert_nonnull (button_down);
  gtk_button_set_image (GTK_BUTTON (button_down), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_down, middle_button_pos,
                 10 + (BUTTON_SIZE * 6));

  //

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  GtkWidget *button_vol_up = gtk_button_new ();
  g_assert_nonnull (button_vol_up);
  gtk_button_set_image (GTK_BUTTON (button_vol_up), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_vol_up, 0,
                 20 + (BUTTON_SIZE * 7));

  button_icon = gtk_image_new_from_file (g_strconcat (
      ginga_gui.executable_folder, "icons/light-theme/menu.png", NULL));
  GtkWidget *button_menu = gtk_button_new ();
  g_assert_nonnull (button_menu);
  gtk_button_set_image (GTK_BUTTON (button_menu), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_menu, BUTTON_SIZE,
                 20 + (BUTTON_SIZE * 7));

  button_icon = gtk_image_new_from_file (g_strconcat (
      ginga_gui.executable_folder, "icons/light-theme/close.png", NULL));
  GtkWidget *button_close = gtk_button_new ();
  g_assert_nonnull (button_close);
  gtk_button_set_image (GTK_BUTTON (button_close), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_close, (BUTTON_SIZE * 2),
                 20 + (BUTTON_SIZE * 7));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  GtkWidget *button_ch_up = gtk_button_new ();
  g_assert_nonnull (button_ch_up);
  gtk_button_set_image (GTK_BUTTON (button_ch_up), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_ch_up, (BUTTON_SIZE * 3),
                 20 + (BUTTON_SIZE * 7));

  //

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  GtkWidget *button_vol_down = gtk_button_new ();
  g_assert_nonnull (button_vol_down);
  gtk_button_set_image (GTK_BUTTON (button_vol_down), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_vol_down, 0,
                 20 + (BUTTON_SIZE * 8));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  GtkWidget *button_menu2 = gtk_button_new ();
  g_assert_nonnull (button_menu2);
  gtk_button_set_image (GTK_BUTTON (button_menu2), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_menu2, BUTTON_SIZE,
                 20 + (BUTTON_SIZE * 8));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  GtkWidget *button_info2 = gtk_button_new ();
  g_assert_nonnull (button_info2);
  gtk_button_set_image (GTK_BUTTON (button_info2), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_info2, (BUTTON_SIZE * 2),
                 20 + (BUTTON_SIZE * 8));

  button_icon = gtk_image_new_from_file (
      g_strconcat (ginga_gui.executable_folder,
                   "icons/light-theme/settings-icon.png", NULL));
  GtkWidget *button_ch_down = gtk_button_new ();
  g_assert_nonnull (button_ch_down);
  gtk_button_set_image (GTK_BUTTON (button_ch_down), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_ch_down,
                 (BUTTON_SIZE * 3), 20 + (BUTTON_SIZE * 8));

  //

  button_icon = gtk_image_new_from_file (g_strconcat (
      ginga_gui.executable_folder, "icons/common/red_icon.png", NULL));
  GtkWidget *button_red = gtk_button_new ();
  g_assert_nonnull (button_red);
  gtk_button_set_image (GTK_BUTTON (button_red), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_red, 0,
                 20 + (BUTTON_SIZE * 9));

  button_icon = gtk_image_new_from_file (g_strconcat (
      ginga_gui.executable_folder, "icons/common/green_icon.png", NULL));
  GtkWidget *button_green = gtk_button_new ();
  g_assert_nonnull (button_green);
  gtk_button_set_image (GTK_BUTTON (button_green), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_green, BUTTON_SIZE,
                 20 + (BUTTON_SIZE * 9));

  button_icon = gtk_image_new_from_file (g_strconcat (
      ginga_gui.executable_folder, "icons/common/yellow_icon.png", NULL));
  GtkWidget *button_yellow = gtk_button_new ();
  g_assert_nonnull (button_yellow);
  gtk_button_set_image (GTK_BUTTON (button_yellow), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_yellow, (BUTTON_SIZE * 2),
                 20 + (BUTTON_SIZE * 9));

  button_icon = gtk_image_new_from_file (g_strconcat (
      ginga_gui.executable_folder, "icons/common/blue_icon.png", NULL));
  GtkWidget *button_blue = gtk_button_new ();
  g_assert_nonnull (button_blue);
  gtk_button_set_image (GTK_BUTTON (button_blue), button_icon);
  gtk_fixed_put (GTK_FIXED (fixed_layout), button_blue, (BUTTON_SIZE * 3),
                 20 + (BUTTON_SIZE * 9));

  GtkWidget *label = gtk_label_new ("...");
  g_assert_nonnull (label);
  gtk_fixed_put (GTK_FIXED (fixed_layout), label, middle_button_pos + 10,
                 20 + (BUTTON_SIZE * 10));

  gtk_container_add (GTK_CONTAINER (tvcontrolWindow), fixed_layout);
  gtk_widget_show_all (tvcontrolWindow);
}

void
destroy_tvcontrol_window (void)
{
  gtk_widget_destroy (tvcontrolWindow);
  tvcontrolWindow = NULL;
}