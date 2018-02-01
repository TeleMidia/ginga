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

GtkWidget *fullscreenWindow = NULL;
gboolean isFullScreenMode = FALSE;

/* Windows */
void
create_fullscreen_window (void)
{
  if (fullscreenWindow != NULL)
    return;

  isFullScreenMode = TRUE;

  GdkRectangle rect;
  GdkDisplay *display = gdk_display_get_default ();
  g_assert_nonnull (display);

#if GTK_CHECK_VERSION(3, 22, 0)
  GdkMonitor *monitor = gdk_display_get_monitor_at_window (
      GDK_DISPLAY (display), GDK_WINDOW (mainWindow));
  g_assert_nonnull (monitor);
  gdk_monitor_get_geometry (GDK_MONITOR (monitor), &rect);
#else
  GdkScreen *screen = gdk_display_get_screen (GDK_DISPLAY (display), 0);
  g_assert_nonnull (screen);
  gdk_screen_get_monitor_geometry (GDK_SCREEN (screen), 0, &rect);
#endif

  fullscreenWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (fullscreenWindow);
  gtk_window_set_title (GTK_WINDOW (fullscreenWindow), "Ginga");
  gtk_window_set_default_size (GTK_WINDOW (fullscreenWindow), rect.width,
                               rect.height);
  gtk_window_set_position (GTK_WINDOW (fullscreenWindow),
                           GTK_WIN_POS_CENTER);

#if GTK_CHECK_VERSION(3, 8, 0)
  gtk_widget_add_tick_callback (
      fullscreenWindow, (GtkTickCallback) update_draw_callback, NULL, NULL);
#else
  g_timeout_add (1000 / 60, (GSourceFunc) update_draw_callback,
                 fullscreenWindow);
#endif

  g_signal_connect (fullscreenWindow, "key-press-event",
                    G_CALLBACK (keyboard_callback), (void *) "press");
  g_signal_connect (fullscreenWindow, "key-release-event",
                    G_CALLBACK (keyboard_callback), (void *) "release");
  gtk_container_set_border_width (GTK_CONTAINER (fullscreenWindow), 0);

  // Create Drawing area
  GtkWidget *canvas = gtk_drawing_area_new ();
  g_assert_nonnull (canvas);
  gtk_widget_set_app_paintable (canvas, TRUE);
  g_signal_connect (canvas, "draw", G_CALLBACK (draw_callback), NULL);
  gtk_widget_set_size_request (canvas, rect.width, rect.height);
  gtk_container_add (GTK_CONTAINER (fullscreenWindow), canvas);

  gtk_window_fullscreen (GTK_WINDOW (fullscreenWindow));

  g_signal_connect (fullscreenWindow, "destroy",
                    G_CALLBACK (destroy_fullscreen_window), NULL);

  gtk_widget_show_all (fullscreenWindow);

  GINGA->resize (rect.width, rect.height);

  // printf("Fullscreen size is: %dx%d", rect.width, rect.height);
}
void
destroy_fullscreen_window (void)
{
  gtk_widget_destroy (fullscreenWindow);
  fullscreenWindow = NULL;
  isFullScreenMode = FALSE;

  GINGA->resize (presentationAttributes.resolutionWidth,
                 presentationAttributes.resolutionHeight);
}

/* Modes */
void
set_fullscreen_mode (void)
{
  create_fullscreen_window ();
}

void
set_unfullscreen_mode (void)
{
  destroy_fullscreen_window ();
}
