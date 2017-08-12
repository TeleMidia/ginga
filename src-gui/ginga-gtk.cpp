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

// Global formatter.
Ginga *GINGA = nullptr;

gchar* executableFolder;

int
main (int argc, char **argv)
{
 // check_updates();
  load_settings ();
  // Create Ginga handle width the original args.
  GINGA = new Ginga (argc, argv, presentationAttributes.resolutionWidth, presentationAttributes.resolutionHeight, false); 
  
  executableFolder = g_strconcat (
      g_get_current_dir (), g_path_get_dirname (argv[0]) + 1, "/", NULL);
 // printf ("PATH: %s \n", executableFolder);

  gtk_init (&argc, &argv);
  
  GError **error;
  gtk_window_set_default_icon_from_file (g_strconcat (executableFolder,
                   "icons/common/ginga_icon.png", NULL), error);

  create_main_window ();

  gtk_main ();

  exit (EXIT_SUCCESS);
}
