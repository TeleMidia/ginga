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

#include "ginga.h"
#include <gtk/gtk.h>

static gboolean
draw_callback (GtkWidget *widget, cairo_t *cr, arg_unused (gpointer data))
{
  int w, h;

  w = gtk_widget_get_allocated_width (widget);
  h = gtk_widget_get_allocated_height (widget);

  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgb (cr, 255, 0, 0);
  cairo_rectangle (cr, 0, 0, w, h);
  cairo_fill (cr);

  return TRUE;
}

int
main (int argc, char **argv)
{
  GtkWidget *app;

  gtk_init (&argc, &argv);

  // Create application window.
  app = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_assert_nonnull (app);
  gtk_window_set_title (GTK_WINDOW (app), PACKAGE_STRING);
  gtk_window_set_default_size (GTK_WINDOW (app), 800, 600);
  gtk_widget_set_app_paintable (app, TRUE);

  // Setup GTK+ callbacks.
  g_signal_connect (app, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  g_signal_connect (app, "draw", G_CALLBACK (draw_callback), NULL);

  // Enter event loop.
  gtk_widget_show_all (app);
  gtk_main ();

  exit (EXIT_SUCCESS);
}
