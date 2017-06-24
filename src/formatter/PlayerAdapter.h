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

#ifndef PLAYER_ADAPTER_H
#define PLAYER_ADAPTER_H

#include "NclEvents.h"
#include "ExecutionObject.h"

GINGA_FORMATTER_BEGIN

class PlayerAdapter
{
public:
  explicit PlayerAdapter (string uri, string mimetype);
  virtual ~PlayerAdapter ();

  Player *getPlayer ();

  bool start ();
  bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();

  string getProperty (AttributionEvent *event);
  void setProperty (const string &name, const string &value);

  void setRect (SDL_Rect);
  SDL_Rect getRect ();
  void setZ (int, int);
  void getZ (int *, int *);

public:
  Player *_player;

};

GINGA_FORMATTER_END

#endif // PLAYER_ADAPTER_H
