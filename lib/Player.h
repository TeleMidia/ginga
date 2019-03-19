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

  /// Possible playback state.
  enum State
  {
    PLAYING,                    ///< Playback in progress.
    PAUSED,                     ///< Playback paused.
    STOPPED,                    ///< No playback.
  };

  /// Creates a new player for media object.
  Player (Media *media);

  /// Destroys player.
  virtual ~Player ();

  /// Gets the playback state of player.
  Player::State getState ();

  /// Gets the URI of the content of player.
  string getURI ();

  /// Sets the URI of the content of player.
  virtual void setURI (const string &uri);

  /// Gets the dimensions of the drawing area of player.
  void getRect (int *x, int *y, int *width, int *height);

  /// Sets dimensions of the drawing area of player.
  void setRect (int x, int y, int width, int height);

  /// Gets end-of-stream flag of player.
  bool getEOS ();

  /// Sets end-of-stream flag of player.
  void setEOS (bool eos);

  /// Starts playback.
  virtual void start ();

  /// Stops playback.
  virtual void stop ();

  /// Pauses playback.
  virtual void pause ();

protected:

  /// The Lua state of the media object of player.
  lua_State *_L;

  /// The media object associated with player.
  Media *_media;

  /// The playback state of player.
  State _state;

  /// The surface of player.
  cairo_surface_t *_surface;

  /// The URI of the content of player.
  string _uri;

  /// The dimensions of player drawing area (x, y, w, h in pixels).
  Rect _rect;

  /// Indicates whether the content of player was exhausted.
  bool _eos;

  /// Flags: indicate attribute changes.
  struct {
    bool uri;
    bool rect;
    bool eos;
  } _flag;

  // TODO ------------------------------------------------------------------
public:
  enum Property
  {
    PROP_UNKNOWN = 0,
    PROP_BACKGROUND,
    PROP_BALANCE,
    PROP_BASS,
    PROP_DEBUG,
    PROP_DURATION,
    PROP_EXPLICIT_DUR,
    PROP_FONT_BG_COLOR,
    PROP_FONT_COLOR,
    PROP_FONT_FAMILY,
    PROP_FONT_SIZE,
    PROP_FONT_STYLE,
    PROP_FONT_VARIANT,
    PROP_FONT_WEIGHT,
    PROP_FREEZE,
    PROP_FREQ,
    PROP_HORZ_ALIGN,
    PROP_MUTE,
    PROP_SPEED,
    PROP_TIME,
    PROP_TRANSPARENCY,
    PROP_TREBLE,
    PROP_VERT_ALIGN,
    PROP_VISIBLE,
    PROP_VOLUME,
    PROP_WAVE,
  };


  string getProperty (const string &);
  void setProperty (const string &, const string &);

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

  bool _dirty;               // true if surface should be reloaded
  list<int> _crop;           // polygon for cropping effect

  map<string, string> _properties; // property table
  struct
  {
    Color bgColor;     // background color
    Time duration;     // explicit duration
    bool visible;      // true if visible
    guint8 alpha;      // alpha
    int zindex;        // z-index
    int zorder;        // z-order
    string focusIndex; // focus index
    string type;       // content mime-type
  } _prop;

protected:
  virtual bool doSetProperty (Property, const string &, const string &);

private:
  void redrawDebuggingInfo (cairo_t *);
};

GINGA_NAMESPACE_END

#endif // GINGA_PLAYER_H
