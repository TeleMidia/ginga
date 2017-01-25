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

#ifndef SDL_EVENT_BUFFER_H
#define SDL_EVENT_BUFFER_H

#include "ginga.h"
#include "SDLInputEvent.h"

GINGA_MB_BEGIN

class SDLEventBuffer
{
private:
  pthread_mutex_t ebMutex;
  vector<SDL_Event> eventBuffer;
  bool capsOn;
  bool shiftOn;
  bool isWaiting;
  pthread_cond_t cond;
  pthread_mutex_t condMutex;

public:
  SDLEventBuffer ();
  virtual ~SDLEventBuffer ();
  static bool checkEvent (Uint32 winId, SDL_Event event);
  void feed (SDL_Event event, bool capsOn, bool shiftOn);
  void postInputEvent (SDLInputEvent *event);
  void waitEvent ();
  SDLInputEvent *getNextEvent ();
  void *getContent ();

private:
  void waitForEvent ();
  bool eventArrived ();
};

GINGA_MB_END

#endif /* SDL_EVENT_BUFFER_H */
