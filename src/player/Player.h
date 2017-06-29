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

#include "PlayerAnimator.h"

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

  static Player *createPlayer (const string &, const string &);

  Player (const string &);
  virtual ~Player ();

  string getURI ();
  PlayerState getState ();
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

  SDL_Rect getRect ();
  void setRect (SDL_Rect);

  void getZ (int *, int *);
  void setZ (int, int);

  double getAlpha ();
  void setAlpha (double);

  SDL_Color getBgColor ();
  void setBgColor (SDL_Color);

  bool getVisible ();
  void setVisible (bool);

  bool getFocus ();
  void setFocus (bool);

  // Callbacks.
  virtual void redraw (SDL_Renderer *);

protected:
  string _uri;                     // source uri
  PlayerState _state;              // current state
  bool _eos;                       // true if content was exhausted

  map<string, string> _properties; // property table
  SDL_Rect _rect;                  // x, y, w, h in pixels
  int _z;                          // z-index
  int _zorder;                     // z-order
  guint8 _alpha;                   // alpha
  SDL_Color _bgColor;              // background color
  bool _visible;                   // true if visible
  bool _focused;                   // true if focused

  SDL_Texture *_texture;           // player texture
  PlayerAnimator _animator;        // associated animator
};

GINGA_PLAYER_END

#endif // PLAYER_H
