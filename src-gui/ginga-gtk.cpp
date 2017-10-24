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
#include "aux-glib.h"
#include <locale.h>

// Global formatter.
Ginga *GINGA = nullptr;

int
main (int argc, char **argv)
{
  GingaOptions opts;

  opts.width = presentationAttributes.resolutionWidth;
  opts.height = presentationAttributes.resolutionHeight;
  opts.debug = false;
  opts.opengl = false;
  GINGA = Ginga::create (argc, argv, &opts);
  g_assert_nonnull (GINGA);
  

  gtk_init (&argc, &argv);

  setlocale (LC_ALL, "C");
  
  GError *err = NULL;
  gtk_window_set_default_icon_from_file (
        g_build_path (G_DIR_SEPARATOR_S, GINGADATADIR,
                      "icons/common/ginga_icon.png", NULL),
        &err);

  if (err != NULL)
    {
      fprintf (stderr, "Error: %s\n", err->message);
      g_error_free (err);
    }
  
  load_settings ();
  create_main_window ();
  gtk_main ();

  exit (EXIT_SUCCESS);
}
