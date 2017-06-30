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

static map<SDL_Keycode, string> overrides =
{
 {SDLK_F5, "INFO"},
 {SDLK_DOWN, "CURSOR_DOWN"},
 {SDLK_LEFT, "CURSOR_LEFT"},
 {SDLK_RIGHT, "CURSOR_RIGHT"},
 {SDLK_UP, "CURSOR_UP"},
 {SDLK_RETURN, "ENTER"},
 {SDLK_F1, "RED"},
 {SDLK_F2, "GREEN"},
 {SDLK_F3, "BLUE"},
 {SDLK_F4, "YELLOW"},
};

bool
ginga_key_table_index (SDL_Keycode key, string *result)
{
  map<SDL_Keycode, string>::iterator it;

  if ((it = overrides.find (key)) != overrides.end ())
    {
      set_if_nonnull (result, it->second);
      return true;
    }

  string name = string (SDL_GetKeyName (key));
  if (name != "")
    {
      set_if_nonnull (result, name);
      return true;
    }

  return false;
}
