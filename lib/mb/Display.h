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

#ifndef DISPLAY_H
#define DISPLAY_H

#include "ginga-internal.h"
#include "Dashboard.h"
#include "IEventListener.h"

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_MB_BEGIN

class Display
{
public:
  Display (int, int);
  ~Display ();

  void getSize (int *, int *);
  void setSize (int, int);

  bool registerEventListener (IEventListener *);
  bool unregisterEventListener (IEventListener *);

  void registerPlayer (Player *);
  void unregisterPlayer (Player *);

  void redraw (cairo_t *);
  void notifyTickListeners (GingaTime, GingaTime, int);
  void notifyKeyListeners (const string &, bool);

private:
  int _width;                   // display width in pixels
  int _height;                  // display height in pixels
  Dashboard _dashboard;         // control panel

  GList *_listeners;            // list of listeners to be notified
  GList *_players;              // list of players to be ticked

  bool add (GList **, gpointer);
  bool remove (GList **, gpointer);
};

// Global display.
extern Display *_Ginga_Display;
#define Ginga_Display\
  GINGA_ASSERT_GLOBAL_NONNULL (_Ginga_Display, ::ginga::mb::Display *)

GINGA_MB_END

#endif /* DISPLAY_H */
