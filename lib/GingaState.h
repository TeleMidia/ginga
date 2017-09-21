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

#ifndef GINGA_PRIVATE_H
#define GINGA_PRIVATE_H

#include "ginga.h"
#include "ginga-internal.h"

GINGA_PLAYER_BEGIN
class Player;
GINGA_PLAYER_END
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN
class Scheduler;
GINGA_FORMATTER_END
using namespace ::ginga::formatter;

class IGingaStateEventListener
{
public:
  virtual void handleTickEvent (GingaTime, GingaTime, int) = 0;
  virtual void handleKeyEvent (const string &key, bool) = 0;
};

class GingaState : public Ginga
{
 public:
  // External API.
  bool start (const string &, string *);
  void stop ();
  void resize (int, int);

  void redraw (cairo_t *);
  void sendKeyEvent (const string &, bool);
  void sendTickEvent (uint64_t, uint64_t, uint64_t);

  const GingaOptions *getOptions ();
  bool getOptionBool (const string &);
  void setOptionBool (const string &, bool);
  int getOptionInt (const string &);
  void setOptionInt (const string &, int);
  string getOptionString (const string &);
  void setOptionString (const string &, string);

  // Internal API.
  GingaState (int, char **, GingaOptions *);
  virtual ~GingaState ();
  Scheduler *getScheduler ();

  bool registerEventListener (IGingaStateEventListener *);
  bool unregisterEventListener (IGingaStateEventListener *);
  void registerPlayer (Player *);
  void unregisterPlayer (Player *);

  void *getData (const string &);
  void setData (const string &, void *);

  static void setOptionDebug (GingaState *, const string &, bool);
  static void setOptionSize (GingaState *, const string &, int);
  static void setOptionBackground (GingaState *, const string &, string);

 private:
  GingaOptions _opts;            // current options
  Scheduler *_scheduler;         // formatter core
  GList *_listeners;             // list of listeners to be notified
  GList *_players;               // list of players to be ticked
  map<string, void *> _userdata; // userdata attached to state

  bool _started;                // true if state was started
  string _ncl_file;             // path to current NCL file
  GingaColor _background;       // current background color.
  uint64_t _last_tick_total;    // last total informed via sendTickEvent
  uint64_t _last_tick_diff;     // last diff informed via sendTickEvent
  uint64_t _last_tick_frameno;  // last frameno informed via sendTickEvent

  bool add (GList **, gpointer);
  bool remove (GList **, gpointer);
};

#endif // GINGA_PRIVATE_H
