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

#ifndef PLAYER_H
#define PLAYER_H

#include "GingaState.h"
#include "PlayerAnimator.h"

class GingaState;

GINGA_PLAYER_BEGIN

class Player
{
public:
  enum PlayerState
    {
     PL_SLEEPING = 1,
     PL_OCCURRING,
     PL_PAUSED,
    };

  static Player *createPlayer (GingaState *, const string &,
                               const string &, const string &);
  Player (GingaState *, const string &, const string &);
  virtual ~Player ();

  string getId ();
  string getURI ();
  PlayerState getState ();

  GingaTime getTime ();
  void incTime (GingaTime);

  bool getEOS ();
  void setEOS (bool);

  virtual void start ();
  virtual void stop ();
  virtual void pause ();
  virtual void resume ();

  void schedulePropertyAnimation (const string &, const string &,
                                  const string &, GingaTime);
  // Properties.
  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &);

  bool isFocused ();

  GingaRect getRect ();
  void setRect (GingaRect);

  void getZ (int *, int *);
  void setZ (int, int);

  double getAlpha ();
  void setAlpha (double);

  GingaColor getBgColor ();
  void setBgColor (GingaColor);

  bool getVisible ();
  void setVisible (bool);

  GingaTime getDuration ();
  void setDuration (GingaTime);

  // Callbacks.
  virtual void reload ();
  virtual void redraw (cairo_t *);

  // Static.
  static string getCurrentFocus ();
  static void setCurrentFocus (const string &);

protected:
  GingaState *_ginga;              // ginga state
  string _id;                      // associated object id
  string _uri;                     // source uri
  PlayerState _state;              // current state
  GingaTime _time;                 // playback time
  bool _eos;                       // true if content was exhausted
  cairo_surface_t *_surface;       // player surface
  bool _dirty;                     // true if surface should be reloaded
  PlayerAnimator *_animator;       // associated animator

  map<string, string> _properties; // property table
  bool _debug;                     // true if debugging mode is on
  string _focusIndex;              // focus index
  GingaRect _rect;                 // x, y, w, h in pixels
  int _z;                          // z-index
  int _zorder;                     // z-order
  guint8 _alpha;                   // alpha
  GingaColor _bgColor;             // background color
  bool _visible;                   // true if visible
  GingaTime _duration;             // explicit duration

private:
  void redrawDebuggingInfo (cairo_t *);

  // Static.
  static string _currentFocus;  // current (global) focus index
};

GINGA_PLAYER_END

#endif // PLAYER_H
