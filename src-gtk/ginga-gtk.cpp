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

// SDL_Window *window;
// SDL_Renderer *renderer;

Ginga_GUI ginga_gui;

int
main (int argc, char **argv)
{

  ginga_gui.executable_folder = g_strconcat (
      g_get_current_dir (), g_path_get_dirname (argv[0]) + 1, "/", NULL);
  printf ("PATH: %s \n", ginga_gui.executable_folder);

  gtk_init (&argc, &argv);
  
  GError **error;
  gtk_window_set_default_icon_from_file (g_strconcat (ginga_gui.executable_folder,
                   "icons/common/ginga_icon.png", NULL), error);

  create_main_window ();

  // SDL TEST
  /* if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
         SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize
   SDL: %s", SDL_GetError());
         return 1;
   }
   window = create_sdl_window_from_gtk_widget(toplevel_window);
   g_assert_nonnull(window);
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE |
   SDL_RENDERER_TARGETTEXTURE);
   g_assert_nonnull(renderer);  */

  // Enter event loop.

  gtk_main ();

  exit (EXIT_SUCCESS);
}
