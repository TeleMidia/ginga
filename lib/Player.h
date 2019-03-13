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

#ifndef GINGA_PLAYER_H
#define GINGA_PLAYER_H

GINGA_NAMESPACE_BEGIN

class Media;
class Player
{
public:

  /// Creates a new player for media object.
  Player (Media *media);

  /// Destroys player.
  virtual ~Player ();

  /// Gets end-of-stream flag of player.
  bool getEOS ();

  /// Sets end-of-stream flag of player.
  void setEOS (bool eos);

  // TODO ------------------------------------------------------------------

  enum State
    {
     OCCURRING,
     SLEEPING,
     PAUSED,
    };

  enum Property
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
    PROP_FREQ,
    PROP_HEIGHT,
    PROP_HORZ_ALIGN,
    PROP_LEFT,
    PROP_LOCATION,
    PROP_MUTE,
    PROP_RIGHT,
    PROP_SIZE,
    PROP_SPEED,
    PROP_TIME,
    PROP_TOP,
    PROP_TRANSPARENCY,
    PROP_TREBLE,
    PROP_TYPE,
    PROP_URI,
    PROP_VERT_ALIGN,
    PROP_VISIBLE,
    PROP_VOLUME,
    PROP_WAVE,
    PROP_WIDTH,
    PROP_Z_INDEX,
    PROP_Z_ORDER,
  };


  string getProperty (const string &);
  void setProperty (const string &, const string &);

  State getState ();

  virtual void start ();
  virtual void stop ();
  virtual void pause ();
  virtual void resume ();

  void resetProperties ();
  void resetProperties (set<string> *);
  void schedulePropertyAnimation (const string &, const string &,
                                  const string &, Time);
  virtual void reload ();
  virtual void redraw (cairo_t *);

  virtual void sendKeyEvent (const string &, bool);

  virtual void
  sendPresentationEvent (const string &, const string &)
  {
  }

  // Static.
  static Property getPlayerProperty (const string &, string *);

protected:

  /// The Lua state of the associated media object.
  lua_State *_L;

  /// The associated media object.
  Media *_media;

  /// The end-of-stream flag; indicates whether player content was
  /// exhausted.
  bool _eos;

  State _state;              // current state

  cairo_surface_t *_surface; // player surface
  bool _dirty;               // true if surface should be reloaded
  list<int> _crop;           // polygon for cropping effect

  map<string, string> _properties; // property table
  struct
  {
    Color bgColor;     // background color
    Rect rect;         // x, y, w, h in pixels
    Time duration;     // explicit duration
    bool debug;        // true if debugging mode is on
    bool visible;      // true if visible
    guint8 alpha;      // alpha
    int zindex;        // z-index
    int zorder;        // z-order
    string focusIndex; // focus index
    string type;       // content mime-type
    string uri;        // content URI
  } _prop;

protected:
  virtual bool doSetProperty (Property, const string &, const string &);

private:
  void redrawDebuggingInfo (cairo_t *);
};

GINGA_NAMESPACE_END

#endif // GINGA_PLAYER_H
