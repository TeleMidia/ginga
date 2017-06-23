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

#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

GINGA_PLAYER_BEGIN

class Player
{
public:
  enum PlayerStatus
    {
     PL_SLEEPING = 1,
     PL_OCCURRING,
     PL_PAUSED,
    };

  enum PlayerNotify
    {
     PL_NOTIFY_START = 1,
     PL_NOTIFY_PAUSE,
     PL_NOTIFY_RESUME,
     PL_NOTIFY_STOP,
     PL_NOTIFY_ABORT,
    };

  enum PlayerEventType
    {
     PL_TYPE_PRESENTATION = 10,
     PL_TYPE_ATTRIBUTION,
     PL_TYPE_SELECTION
    };

public:
  Player (const string &mrl);
  virtual ~Player ();

  void setAnimatorProperties (string dur, string name, string value);

  PlayerStatus getMediaStatus();

  virtual bool play ();
  virtual void stop ();
  virtual void abort ();
  virtual void pause ();
  virtual void resume ();
  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &);

  virtual void setOutWindow (SDLWindow *);

  virtual void redraw (SDL_Renderer *);

protected:
  PlayerStatus status;
  string mrl;
  SDLWindow *window;
  PlayerAnimator *animator;
  bool presented;

  // Media attributes.
  SDL_Texture *texture;
  int borderWidth;
  SDL_Color bgColor;
  SDL_Color borderColor;
  guint8 alpha;

private:
  map<string, string> _properties;

  // --------------------------------------------------------------------------

public:
  void setRect (SDL_Rect);
  SDL_Rect getRect ();
  void setZ (int, int);
  void getZ (int *, int *);

  void setFocus (bool);
  bool getFocus ();

protected:
  SDL_Rect _rect;               // x, y, w, h in pixels
  int _z;                       // z-index
  int _zorder;                  // z-order

  bool _focused;                // true if player has focus
};

GINGA_PLAYER_END

#endif /* PLAYER_H */
