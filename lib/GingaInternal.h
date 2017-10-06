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

#ifndef GINGA_INTERNAL_H
#define GINGA_INTERNAL_H

#include "ginga.h"
#include "aux-ginga.h"
#include "aux-gl.h"

GINGA_PLAYER_BEGIN
class Player;
GINGA_PLAYER_END
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN
class Scheduler;
GINGA_FORMATTER_END
using namespace ::ginga::formatter;

class IGingaInternalEventListener
{
public:
  virtual void handleTickEvent (GingaTime, GingaTime, int) = 0;
  virtual void handleKeyEvent (const string &key, bool) = 0;
};

class GingaInternal : public Ginga
{
 public:
  // External API.
  GingaState getState ();

  bool start (const string &, string *);
  bool stop ();

  void resize (int, int);
  void redraw (cairo_t *);
  void redrawGL ();

  bool sendKeyEvent (const string &, bool);
  bool sendTickEvent (uint64_t, uint64_t, uint64_t);

  const GingaOptions *getOptions ();
  bool getOptionBool (const string &);
  void setOptionBool (const string &, bool);
  int getOptionInt (const string &);
  void setOptionInt (const string &, int);
  string getOptionString (const string &);
  void setOptionString (const string &, string);

  // Internal API.
  GingaInternal (int, char **, GingaOptions *);
  virtual ~GingaInternal ();
  Scheduler *getScheduler ();

  bool getEOS ();
  void setEOS (bool);

  bool registerEventListener (IGingaInternalEventListener *);
  bool unregisterEventListener (IGingaInternalEventListener *);

  void registerPlayer (Player *);
  void unregisterPlayer (Player *);

  void *getData (const string &);
  void setData (const string &, void *);

  static void setOptionDebug (GingaInternal *, const string &, bool);
  static void setOptionExperimental (GingaInternal *, const string &, bool);
  static void setOptionOpenGL (GingaInternal *, const string &, bool);
  static void setOptionSize (GingaInternal *, const string &, int);
  static void setOptionBackground (GingaInternal *, const string &, string);

 private:
  GingaState _state;             // current state
  GingaOptions _opts;            // current options
  Scheduler *_scheduler;         // formatter core
  GList *_listeners;             // list of listeners to be notified
  GList *_players;               // list of players to be ticked
  map<string, void *> _userdata; // userdata attached to state

  string _ncl_file;               // path to current NCL file
  bool _eos;                      // true if EOS was reached
  GingaColor _background;         // current background color
  uint64_t _last_tick_total;      // last total informed via sendTickEvent
  uint64_t _last_tick_diff;       // last diff informed via sendTickEvent
  uint64_t _last_tick_frameno;    // last frameno informed via sendTickEvent
  string _saved_G_MESSAGES_DEBUG; // saved G_MESSAGES_DEBUG value

  bool add (GList **, gpointer);
  bool remove (GList **, gpointer);
};

#endif // GINGA_PRIVATE_H
