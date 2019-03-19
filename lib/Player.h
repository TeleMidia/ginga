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

  /// Gets the dimensions of the visual output of player.
  void getRect (int *x, int *y, int *width, int *height);

  /// Sets dimensions of the visual output of player.
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

  /// Draws the visual output of player into Cairo context.
  virtual void draw (cairo_t *cr);

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

  /// The dimensions of player's visual output  (x, y, w, h in pixels).
  Rect _rect;

  /// Whether the content of player was exhausted.
  bool _eos;

  /// The alpha to be applied to player's visual output.
  guint8 _alpha;

  /// The color of the background of player's visual output.
  Color _bgColor;
};

GINGA_NAMESPACE_END

#endif // GINGA_PLAYER_H
