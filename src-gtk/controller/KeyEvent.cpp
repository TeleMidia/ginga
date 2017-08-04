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

gboolean isCrtlModifierActive = FALSE;

void
key_press_event_callback (GtkWidget *widget, GdkEventKey *event)
{
  gchar *key_name = gdk_keyval_name (event->keyval);

  if (!g_strcmp0 (key_name, "Control_L")
      || !g_strcmp0 (key_name, "Control_R")
      || !g_strcmp0 (key_name, "Meta_R") || !g_strcmp0 (key_name, "Meta_L"))
    isCrtlModifierActive = TRUE;
  else if ((!g_strcmp0 (key_name, "r") || !g_strcmp0 (key_name, "R"))
           && isCrtlModifierActive && !isFullScreenMode)
    create_tvcontrol_window ();
  else if ((!g_strcmp0 (key_name, "d") || !g_strcmp0 (key_name, "D"))
           && isCrtlModifierActive && !isFullScreenMode)
    enable_disable_debug ();
  else if ((!g_strcmp0 (key_name, "f") || !g_strcmp0 (key_name, "F"))
           && isCrtlModifierActive && !isFullScreenMode)
    set_fullscreen_mode ();
  else if (!g_strcmp0 (key_name, "Escape") && isFullScreenMode)
    set_unfullscreen_mode ();
}

void
key_release_event_callback (GtkWidget *widget, GdkEventKey *event)
{
  gchar *key_name = gdk_keyval_name (event->keyval);

  if (!g_strcmp0 (key_name, "Control_L")
      || !g_strcmp0 (key_name, "Control_R")
      || !g_strcmp0 (key_name, "Meta_R") || !g_strcmp0 (key_name, "Meta_L"))
    isCrtlModifierActive = FALSE;
}