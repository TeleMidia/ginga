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

static map<SDL_Keycode, string> keytab =
{
 {SDLK_ESCAPE, "QUIT"},
 {SDLK_0, "0" },
 {SDLK_1, "1"},
 {SDLK_2, "2"},
 {SDLK_3, "3"},
 {SDLK_4, "4"},
 {SDLK_5, "5"},
 {SDLK_6, "6"},
 {SDLK_7, "7"},
 {SDLK_8, "8"},
 {SDLK_9, "9"},
 {SDLK_a, "a"},
 {SDLK_b, "b"},
 {SDLK_c, "c"},
 {SDLK_d, "d"},
 {SDLK_e, "e"},
 {SDLK_f, "f"},
 {SDLK_g, "g"},
 {SDLK_h, "h"},
 {SDLK_i, "i"},
 {SDLK_j, "j"},
 {SDLK_k, "k"},
 {SDLK_l, "l"},
 {SDLK_m, "m"},
 {SDLK_n, "n"},
 {SDLK_o, "o"},
 {SDLK_p, "p"},
 {SDLK_q, "q"},
 {SDLK_r, "r"},
 {SDLK_s, "s"},
 {SDLK_t, "t"},
 {SDLK_u, "u"},
 {SDLK_v, "v"},
 {SDLK_w, "w"},
 {SDLK_x, "x"},
 {SDLK_y, "y"},
 {SDLK_z, "z"},
 {SDLK_ASTERISK, "*"},
 {SDLK_HASH, "#"},
 {SDLK_PERIOD, "."},
 {SDLK_F5, "MENU"},
 {SDLK_F6, "INFO"},
 {SDLK_DOWN, "CURSOR_DOWN"},
 {SDLK_LEFT, "CURSOR_LEFT"},
 {SDLK_RIGHT, "CURSOR_RIGHT"},
 {SDLK_UP, "CURSOR_UP"},
 {SDLK_PAGEDOWN, "CHANNEL_DOWN"},
 {SDLK_PAGEUP, "CHANNEL_UP"},
 {SDLK_PLUS, "VOLUME_DOWN"},
 {SDLK_MINUS, "VOLUME_UP"},
 {SDLK_RETURN, "ENTER"},
 {SDLK_F1, "RED"},
 {SDLK_F2, "GREEN"},
 {SDLK_F3, "YELLOW"},
 {SDLK_F4, "BLUE"},
 {SDLK_TAB, "\t"},
 {SDLK_SPACE, " "},
 {SDLK_BACKSPACE, "BACK"},
 {SDLK_POWER, "POWER"},
 {SDLK_STOP, "STOP"},
 {SDLK_EJECT, "EJECT"},
 {SDLK_AUDIOPLAY, "PLAY"},
 {SDLK_PAUSE, "PAUSE"},
};

bool
ginga_key_table_index (SDL_Keycode key, string &result)
{
  map<SDL_Keycode, string>::iterator it;
  if ((it = keytab.find (key)) == keytab.end ())
    return false;
  result = it->second;
  return true;
}
