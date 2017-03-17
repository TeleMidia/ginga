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

#include "IKeyInputEventListener.h"

static map<SDL_Keycode, string> _keyMap
    = { { SDLK_ESCAPE, "QUIT" },
   //     { "NO_CODE", SDLK_END }, // inexist
        { SDLK_0, "0"  },
        { SDLK_1, "1" },
        { SDLK_2, "2" },
        { SDLK_3, "3" },
        { SDLK_4, "4" },
        { SDLK_5, "5" },
        { SDLK_6, "6" },
        { SDLK_7, "7" },
        { SDLK_8, "8" },
        { SDLK_9, "9" },

        { SDLK_a, "a" },
        { SDLK_b, "b" },
        { SDLK_c, "c" },
        { SDLK_d, "d" },
        { SDLK_e, "e" },
        { SDLK_f, "f" },
        { SDLK_g, "g" },
        { SDLK_h, "h" },
        { SDLK_i, "i" },
        { SDLK_j, "j" },
        { SDLK_k, "k" },
        { SDLK_l, "l" },
        { SDLK_m, "m" },
        { SDLK_n, "n" },
        { SDLK_o, "o" },
        { SDLK_p, "p" },
        { SDLK_q, "q" },
        { SDLK_r, "r" },
        { SDLK_s, "s" },
        { SDLK_t, "t" },
        { SDLK_u, "u" },
        { SDLK_v, "v" },
        { SDLK_w, "w" },
        { SDLK_x, "x" },
        { SDLK_y, "y" },
        { SDLK_z, "z" },
    /*    { "A", SDLK_a },
        { "B", SDLK_b },
        { "C", SDLK_c },
        { "D", SDLK_d },
        { "E", SDLK_e },
        { "F", SDLK_f },
        { "G", SDLK_g },
        { "H", SDLK_h },
        { "I", SDLK_i },
        { "J", SDLK_j },
        { "K", SDLK_k },
        { "L", SDLK_l },
        { "M", SDLK_m },
        { "N", SDLK_n },
        { "O", SDLK_o },
        { "P", SDLK_p },
        { "Q", SDLK_q },
        { "R", SDLK_r },
        { "S", SDLK_s },
        { "T", SDLK_t },
        { "U", SDLK_u },
        { "V", SDLK_v },
        { "W", SDLK_w },
        { "X", SDLK_x },
        { "Y", SDLK_y },
        { "Z", SDLK_z }, */

        { SDLK_ASTERISK, "*" },
        { SDLK_HASH, "#" },
        { SDLK_PERIOD, "." },

        { SDLK_F5, "MENU" },
        { SDLK_F6, "INFO" },
   //     { "GUIDE", SDLK_F5 }, //inexist

        { SDLK_DOWN, "CURSOR_DOWN" },
        { SDLK_LEFT, "CURSOR_LEFT" },
        { SDLK_RIGHT, "CURSOR_RIGHT" },
        { SDLK_UP, "CURSOR_UP" },

        { SDLK_PAGEDOWN, "CHANNEL_DOWN" },
        { SDLK_PAGEUP, "CHANNEL_UP" },

        { SDLK_PLUS, "VOLUME_DOWN" },
        { SDLK_MINUS, "VOLUME_UP" },

        { SDLK_RETURN, "ENTER" },
   //     { "TAP", KEY_TAP },  //inexist

        { SDLK_F1, "RED" },
        { SDLK_F2, "GREEN" },
        { SDLK_F3, "YELLOW" },
        { SDLK_F4, "BLUE" },

        { SDLK_TAB, "	" },
        { SDLK_SPACE, " " },
        { SDLK_BACKSPACE, "BACK" },
     //   { "EXIT", KEY_EXIT }, //inexist

        { SDLK_POWER, "POWER" },
     //   { "REWIND", KEY_REWIND },
        { SDLK_STOP, "STOP" },
        { SDLK_EJECT, "EJECT" },
        { SDLK_AUDIOPLAY, "PLAY" },
    //    { "RECORD", KEY_RECORD }, //inexist
        { SDLK_PAUSE, "PAUSE" } };

const char * convertSdl2GingaKey(SDL_Keycode key){
      map<SDL_Keycode, string>::iterator it;
      it = _keyMap.find(key);
      if (it != _keyMap.end()) 
           return (it->second).c_str();
      else 
           return "NO_CODE";     
}