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

private:
  pthread_mutex_t listM;
  bool notifying;

  map<string, string> properties;
  set<IPlayerListener *> listeners;

protected:
  PlayerStatus status;
  string mrl;
  SDLWindow *window;
  PlayerAnimator *animator;
  bool presented;
  bool visible;
  bool forcedNaturalEnd;
  PlayerEventType scopeType;
  string scope;
  double scopeInitTime;
  double scopeEndTime;

  // Time attributes.
  guint32 initStartTime;
  guint32 initPauseTime;
  guint32 accTimePlaying;
  guint32 accTimePaused;

  // Media attributes.
  SDL_Texture *texture;         // media texture
  int borderWidth;
  SDL_Color bgColor;            // background color
  SDL_Color borderColor;
  SDL_Rect rect;                // draw area
  int z;                        // z-index
  int zorder;
  guint8 alpha;

public:
  Player (const string &mrl);
  virtual ~Player ();

public:
  virtual void setMrl (const string &mrl, bool visible = true);
  virtual void addListener (IPlayerListener *listener);
  void removeListener (IPlayerListener *listener);
  void getZ (int *, int *);
  void setAnimatorProperties (string dur, string name, string value);

public:
  void redraw (SDL_Renderer*);

public:
  void notifyPlayerListeners (short code,
                              const string &parameter,
                              PlayerEventType type,
                              const string &value);
public:
  virtual void setMediaTime (guint32 newTime);

  PlayerStatus getMediaStatus();

  guint32 getMediaTime ();
  virtual double getTotalMediaTime ();

  virtual void setScope (const string &scope,
                         PlayerEventType type = PL_TYPE_PRESENTATION,
                         double begin = -1, double end = -1);
public:

  virtual bool play ();
  virtual void stop ();
  virtual void abort ();
  virtual void pause ();
  virtual void resume ();
  virtual string getPropertyValue (const string &name);
  virtual void setPropertyValue (const string &name, const string &value);
  bool isVisible ();
  void setVisible (bool visible);

public:
  void forceNaturalEnd (bool forceIt);
  bool isForcedNaturalEnd ();
  virtual void setOutWindow (SDLWindow *);

  // Application player only.
  virtual void setCurrentScope (arg_unused (const string &scopeId)){};
};

GINGA_PLAYER_END

#endif /* PLAYER_H */
