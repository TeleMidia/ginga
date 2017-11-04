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

#include "Formatter.h"
#include "PlayerAnimator.h"

GINGA_NAMESPACE_BEGIN

class Formatter;
class Player
{
public:
  enum PlayerState
    {
     SLEEPING = 1,              // stopped
     OCCURRING,                 // playing
     PAUSED,                    // paused
    };

  enum PlayerProperty           // known properties
    {
     PROP_UNKNOWN = 0,
     PROP_BACKGROUND,
     PROP_BALANCE,
     PROP_BASS,
     PROP_BOTTOM,
     PROP_BOUNDS,
     PROP_DEBUG,
     PROP_DURATION,
     PROP_EXPLICIT_DUR,
     PROP_FOCUS_INDEX,
     PROP_FONT_BG_COLOR,
     PROP_FONT_COLOR,
     PROP_FONT_FAMILY,
     PROP_FONT_SIZE,
     PROP_FONT_STYLE,
     PROP_FONT_VARIANT,
     PROP_FONT_WEIGHT,
     PROP_FREEZE,
     PROP_HEIGHT,
     PROP_HORZ_ALIGN,
     PROP_LEFT,
     PROP_LOCATION,
     PROP_MUTE,
     PROP_RIGHT,
     PROP_SIZE,
     PROP_TOP,
     PROP_TRANSPARENCY,
     PROP_TREBLE,
     PROP_VERT_ALIGN,
     PROP_VISIBLE,
     PROP_VOLUME,
     PROP_WIDTH,
     PROP_Z_INDEX,
    };

  Player (Formatter *, const string &, const string &);
  virtual ~Player ();

  PlayerState getState ();
  bool isFocused ();

  GingaTime getTime ();
  void incTime (GingaTime);

  GingaTime getDuration ();
  void setDuration (GingaTime);

  bool getEOS ();
  void setEOS (bool);

  void getZ (int *, int *);
  void setZ (int, int);

  virtual void start ();
  virtual void stop ();
  virtual void pause ();
  virtual void resume ();

  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &);
  void resetProperties ();
  void resetProperties (set<string> *);
  void schedulePropertyAnimation (const string &, const string &,
                                  const string &, GingaTime);
  virtual void reload ();
  virtual void redraw (cairo_t *);

  virtual void sendKeyEvent (const string &, bool);

  // Static.
  static string getCurrentFocus ();
  static void setCurrentFocus (const string &);
  static PlayerProperty getPlayerProperty (const string &, string *);
  static Player *createPlayer (Formatter *, const string &,
                               const string &, const string &);
protected:
  Formatter *_formatter;        // formatter handle
  bool _opengl;                 // true if OpenGL is used
  string _id;                   // associated object id
  string _uri;                  // source uri
  PlayerState _state;           // current state
  GingaTime _time;              // playback time
  bool _eos;                    // true if content was exhausted
  cairo_surface_t *_surface;    // player surface
  guint _gltexture;             // OpenGL texture (if OpenGL is used)
  bool _dirty;                  // true if surface should be reloaded
  PlayerAnimator *_animator;    // associated animator

  map<string, string> _properties; // property table
  struct
  {
    bool debug;                 // true if debugging mode is on
    string focusIndex;          // focus index
    GingaRect rect;             // x, y, w, h in pixels
    int z;                      // z-index
    int zorder;                 // z-order
    guint8 alpha;               // alpha
    GingaColor bgColor;         // background color
    bool visible;               // true if visible
    GingaTime duration;         // explicit duration
  } _prop;

protected:
  virtual bool doSetProperty (PlayerProperty, const string &,
                              const string &);
private:
  void redrawDebuggingInfo (cairo_t *);

  // Static.
  static string _currentFocus;  // current (global) focus index
};

GINGA_NAMESPACE_END

#endif // PLAYER_H
