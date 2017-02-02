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

#ifndef CODE_MAP_H
#define CODE_MAP_H

#include "ginga.h"
#include <unordered_map>

GINGA_MB_BEGIN

class MbKey
{
public:
  enum KeyCode
  {
    KEY_QUIT = -1,
    KEY_NULL = 1,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_SMALL_A,
    KEY_SMALL_B,
    KEY_SMALL_C,
    KEY_SMALL_D,
    KEY_SMALL_E,
    KEY_SMALL_F,
    KEY_SMALL_G,
    KEY_SMALL_H,
    KEY_SMALL_I,
    KEY_SMALL_J,
    KEY_SMALL_K,
    KEY_SMALL_L,
    KEY_SMALL_M,
    KEY_SMALL_N,
    KEY_SMALL_O,
    KEY_SMALL_P,
    KEY_SMALL_Q,
    KEY_SMALL_R,
    KEY_SMALL_S,
    KEY_SMALL_T,
    KEY_SMALL_U,
    KEY_SMALL_V,
    KEY_SMALL_W,
    KEY_SMALL_X,
    KEY_SMALL_Y,
    KEY_SMALL_Z,

    KEY_CAPITAL_A,
    KEY_CAPITAL_B,
    KEY_CAPITAL_C,
    KEY_CAPITAL_D,
    KEY_CAPITAL_E,
    KEY_CAPITAL_F,
    KEY_CAPITAL_G,
    KEY_CAPITAL_H,
    KEY_CAPITAL_I,
    KEY_CAPITAL_J,
    KEY_CAPITAL_K,
    KEY_CAPITAL_L,
    KEY_CAPITAL_M,
    KEY_CAPITAL_N,
    KEY_CAPITAL_O,
    KEY_CAPITAL_P,
    KEY_CAPITAL_Q,
    KEY_CAPITAL_R,
    KEY_CAPITAL_S,
    KEY_CAPITAL_T,
    KEY_CAPITAL_U,
    KEY_CAPITAL_V,
    KEY_CAPITAL_W,
    KEY_CAPITAL_X,
    KEY_CAPITAL_Y,
    KEY_CAPITAL_Z,

    KEY_PAGE_DOWN,
    KEY_PAGE_UP,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    KEY_PLUS_SIGN,
    KEY_MINUS_SIGN,

    KEY_ASTERISK,
    KEY_NUMBER_SIGN,

    KEY_PERIOD,

    KEY_SUPER,
    KEY_PRINTSCREEN,
    KEY_MENU,
    KEY_INFO,
    KEY_EPG,

    KEY_CURSOR_DOWN,
    KEY_CURSOR_LEFT,
    KEY_CURSOR_RIGHT,
    KEY_CURSOR_UP,

    KEY_CHANNEL_DOWN,
    KEY_CHANNEL_UP,

    KEY_VOLUME_DOWN,
    KEY_VOLUME_UP,

    KEY_ENTER,

    KEY_RED,
    KEY_GREEN,
    KEY_YELLOW,
    KEY_BLUE,

    KEY_SPACE,
    KEY_BACKSPACE,
    KEY_BACK,
    KEY_ESCAPE,
    KEY_EXIT,

    KEY_POWER,
    KEY_REWIND,
    KEY_STOP,
    KEY_EJECT,
    KEY_PLAY,
    KEY_RECORD,
    KEY_PAUSE,

    KEY_GREATER_THAN_SIGN,
    KEY_LESS_THAN_SIGN,

    KEY_TAB,
    KEY_TAP,
  };

public:
  static MbKey *getInstance ();

  MbKey::KeyCode getCode (const string &codeStr);
  string getName (MbKey::KeyCode value);
  unordered_map<string, MbKey::KeyCode> *cloneKeyMap ();

private:
  static MbKey *_instance;

  static unordered_map<string, MbKey::KeyCode> _keyMap;
  static unordered_map<MbKey::KeyCode, string, std::hash<int>> _valueMap;

  MbKey () { };
};

GINGA_MB_END

#endif /* CODE_MAP_H */
