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

#include "IPlayerListener.h"
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

  virtual void setMrl (const string &mrl, bool visible = true);
  virtual void addListener (IPlayerListener *listener);
  void removeListener (IPlayerListener *listener);
  void getZ (int *, int *);
  void setAnimatorProperties (string dur, string name, string value);

  void notifyPlayerListeners (short code,
                              const string &parameter,
                              PlayerEventType type,
                              const string &value);



  PlayerStatus getMediaStatus();

  GingaTime getMediaTime ();
  void incMediaTime (GingaTime);
  virtual void setMediaTime (GingaTime);

  virtual void setScope (const string &scope,
                         PlayerEventType type = PL_TYPE_PRESENTATION,
                         GingaTime begin = 0,
                         GingaTime end = GINGA_TIME_NONE);

  virtual bool play ();
  virtual void stop ();
  virtual void abort ();
  virtual void pause ();
  virtual void resume ();
  virtual string getProperty (const string &);
  virtual void setProperty (const string &, const string &);

  bool isVisible ();
  void setVisible (bool visible);

  void forceNaturalEnd (bool forceIt);
  bool isForcedNaturalEnd ();
  virtual void setOutWindow (SDLWindow *);
  virtual void setCurrentScope (arg_unused (const string &scopeId)) {}

  virtual void redraw (SDL_Renderer *);

protected:
  GingaTime _time;              // playback time

  PlayerStatus status;
  string mrl;
  SDLWindow *window;
  PlayerAnimator *animator;
  bool presented;
  bool visible;
  bool forcedNaturalEnd;
  PlayerEventType scopeType;
  string scope;
  GingaTime scopeInitTime;
  GingaTime scopeEndTime;

  // Media attributes.
  SDL_Texture *texture;
  int borderWidth;
  SDL_Color bgColor;
  SDL_Color borderColor;
  SDL_Rect rect;
  int z;
  int zorder;
  guint8 alpha;

private:
  bool _notifying;
  map<string, string> _properties;
  set<IPlayerListener *> _listeners;
};

GINGA_PLAYER_END

#endif /* PLAYER_H */
