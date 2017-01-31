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
#include "CodeMap.h"

GINGA_MB_BEGIN

CodeMap *CodeMap::_instance = NULL;

map <string, int> CodeMap::keyMap = {
  {"QUIT", KEY_QUIT},
  {"NO_CODE", KEY_NULL},
  {"0", KEY_0},
  {"1", KEY_1},
  {"2", KEY_2},
  {"3", KEY_3},
  {"4", KEY_4},
  {"5", KEY_5},
  {"6", KEY_6},
  {"7", KEY_7},
  {"8", KEY_8},
  {"9", KEY_9},

  {"a", KEY_SMALL_A},
  {"b", KEY_SMALL_B},
  {"c", KEY_SMALL_C},
  {"d", KEY_SMALL_D},
  {"e", KEY_SMALL_E},
  {"f", KEY_SMALL_F},
  {"g", KEY_SMALL_G},
  {"h", KEY_SMALL_H},
  {"i", KEY_SMALL_I},
  {"j", KEY_SMALL_J},
  {"k", KEY_SMALL_K},
  {"l", KEY_SMALL_L},
  {"m", KEY_SMALL_M},
  {"n", KEY_SMALL_N},
  {"o", KEY_SMALL_O},
  {"p", KEY_SMALL_P},
  {"q", KEY_SMALL_Q},
  {"r", KEY_SMALL_R},
  {"s", KEY_SMALL_S},
  {"t", KEY_SMALL_T},
  {"u", KEY_SMALL_U},
  {"v", KEY_SMALL_V},
  {"w", KEY_SMALL_W},
  {"x", KEY_SMALL_X},
  {"y", KEY_SMALL_Y},
  {"z", KEY_SMALL_Z},
  {"A", KEY_CAPITAL_A},
  {"B", KEY_CAPITAL_B},
  {"C", KEY_CAPITAL_C},
  {"D", KEY_CAPITAL_D},
  {"E", KEY_CAPITAL_E},
  {"F", KEY_CAPITAL_F},
  {"G", KEY_CAPITAL_G},
  {"H", KEY_CAPITAL_H},
  {"I", KEY_CAPITAL_I},
  {"J", KEY_CAPITAL_J},
  {"K", KEY_CAPITAL_K},
  {"L", KEY_CAPITAL_L},
  {"M", KEY_CAPITAL_M},
  {"N", KEY_CAPITAL_N},
  {"O", KEY_CAPITAL_O},
  {"P", KEY_CAPITAL_P},
  {"Q", KEY_CAPITAL_Q},
  {"R", KEY_CAPITAL_R},
  {"S", KEY_CAPITAL_S},
  {"T", KEY_CAPITAL_T},
  {"U", KEY_CAPITAL_U},
  {"V", KEY_CAPITAL_V},
  {"W", KEY_CAPITAL_W},
  {"X", KEY_CAPITAL_X},
  {"Y", KEY_CAPITAL_Y},
  {"Z", KEY_CAPITAL_Z},

  {"*", KEY_ASTERISK},
  {"#", KEY_NUMBER_SIGN},
  {".", KEY_PERIOD},

  {"MENU", KEY_MENU},
  {"INFO", KEY_INFO},
  {"GUIDE", KEY_EPG},

  {"CURSOR_DOWN", KEY_CURSOR_DOWN},
  {"CURSOR_LEFT", KEY_CURSOR_LEFT},
  {"CURSOR_RIGHT", KEY_CURSOR_RIGHT},
  {"CURSOR_UP", KEY_CURSOR_UP},

  {"CHANNEL_DOWN", KEY_CHANNEL_DOWN},
  {"CHANNEL_UP", KEY_CHANNEL_UP},

  {"VOLUME_DOWN", KEY_VOLUME_DOWN},
  {"VOLUME_UP", KEY_VOLUME_UP},

  {"ENTER", KEY_ENTER},
  {"TAP", KEY_TAP},

  {"RED", KEY_RED},
  {"GREEN", KEY_GREEN},
  {"YELLOW", KEY_YELLOW},
  {"BLUE", KEY_BLUE},

  {"	", KEY_TAB},
  {" ", KEY_SPACE},
  {"BACK", KEY_BACK},
  {"EXIT", KEY_EXIT},

  {"POWER", KEY_POWER},
  {"REWIND", KEY_REWIND},
  {"STOP", KEY_STOP},
  {"EJECT", KEY_EJECT},
  {"PLAY", KEY_PLAY},
  {"RECORD", KEY_RECORD},
  {"PAUSE", KEY_PAUSE}
};

CodeMap::CodeMap ()
{
  map<string, int>::iterator it;
  for (it = keyMap.begin (); it != keyMap.end (); ++it)
  {
    valueMap[it->second] = it->first;
  }
}

CodeMap *
CodeMap::getInstance ()
{
  if (_instance == NULL)
    {
      _instance = new CodeMap ();
    }

  return _instance;
}

int
CodeMap::getCode (const string &codeStr)
{
  if (keyMap.count (codeStr) == 0)
    {
      return KEY_NULL;
    }

  return keyMap[codeStr];
}

string
CodeMap::getValue (int code)
{
  map<int, string>::iterator i;

  i = valueMap.find (code);
  if (i != valueMap.end ())
    {
      return i->second;
    }

  return "";
}

map<string, int> *
CodeMap::cloneKeyMap ()
{
  return new map<string, int> (keyMap);
}

GINGA_MB_END
