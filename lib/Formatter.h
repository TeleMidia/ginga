/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef FORMATTER_H
#define FORMATTER_H

#include "ginga.h"
#include "aux-ginga.h"

#include "Document.h"

GINGA_NAMESPACE_BEGIN

class Context;
class Event;
class Media;
class MediaSettings;
class Object;

class Formatter : public Ginga
{
public:
  lua_State *getLuaState ();
  GingaState getState ();

  bool start (const std::string &, int, int, std::string *);
  bool stop ();

  void resize (int, int);

  bool sendKey (const std::string &, bool);
  bool sendTick (uint64_t, uint64_t, uint64_t);

  string debug_getDocPath ();
  uint64_t debug_getLastTickDiff ();
  uint64_t debug_getLastTickFrame ();
  uint64_t debug_getLastTickTime ();

  Formatter ();
  ~Formatter ();

  void *getDocument ();
  bool getEOS ();
  void setEOS (bool);

private:
  GingaState _state;
  Time _lastTickTime;
  Time _lastTickDiff;
  uint64_t _lastTickFrame;
  string _saved_G_MESSAGES_DEBUG;
  Document *_doc;
  string _docPath;
  bool _eos;
};

GINGA_NAMESPACE_END

#endif // FORMATTER_H
