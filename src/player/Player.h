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

#include "IPlayer.h"
#include "PlayerAnimator.h"

GINGA_PLAYER_BEGIN

enum PLAYER_STATUS
{
  SLEEPING,
  OCCURRING,
  PAUSED
};

typedef struct LockedPlayerLitenerAction
{
  ::ginga::player::IPlayerListener *l;
  bool isAdd;
} LockedPlayerListener;

class Player : public IPlayer
{
private:
  pthread_mutex_t listM;
  pthread_mutex_t lockedListM;
  pthread_mutex_t referM;

  bool notifying;

  map<string, string> properties;
  set<IPlayerListener *> listeners;
  vector<LockedPlayerListener *> lockedListeners;

protected:
  PLAYER_STATUS status;

  string mrl;
  SDLWindow *window;
  SDLWindow *outputWindow;
  PlayerAnimator* animator;
  set<IPlayer *> referredPlayers;
  IPlayer *timeBasePlayer;
  bool presented;
  bool visible;
  bool forcedNaturalEnd;
  string scope;
  short scopeType;
  double scopeInitTime;
  double scopeEndTime;
  double outTransTime;

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
  gint z;                       // z-index
  guint8 alpha;

public:
  Player (const string &mrl);
  virtual ~Player ();

public:
  virtual void setMrl (const string &mrl, bool visible = true);
  virtual void addListener (IPlayerListener *listener);
  void removeListener (IPlayerListener *listener);
  gint getZ();
  void setAnimatorProperties(string dur, string name, string value);

private:
  void performLockedListenersRequest ();

public:
  void redraw (SDL_Renderer*);

public:
  void notifyPlayerListeners (short code,
                              const string &parameter = "",
                              short type = TYPE_PRESENTATION,
                              const string &value = "");


private:
  static void ntsNotifyPlayerListeners (set<IPlayerListener *> *list,
                                        short code,
                                        const string &parameter,
                                        short type,
                                        const string &value);

public:
  virtual void setMediaTime (guint32 newTime);

PLAYER_STATUS getMediaStatus();

  guint32 getMediaTime ();
  virtual double getTotalMediaTime ();

  virtual void setScope (const string &scope, short type = TYPE_PRESENTATION,
                         double begin = -1, double end = -1,
                         double outTransDur = -1);

public:

  virtual bool play ();
  virtual void stop ();
  virtual void abort ();
  virtual void pause ();
  virtual void resume ();
  virtual string getPropertyValue (const string &name);
  virtual void setPropertyValue (const string &name, const string &value);
  virtual void setReferenceTimePlayer (arg_unused (IPlayer *player)){};
  void addTimeReferPlayer (IPlayer *referPlayer);
  void removeTimeReferPlayer (IPlayer *referPlayer);
  void notifyReferPlayers (int transition);
  void timebaseObjectTransitionCallback (int transition);
  void setTimeBasePlayer (IPlayer *timeBasePlayer);
  bool isVisible ();
  void setVisible (bool visible);

public:
  void forceNaturalEnd (bool forceIt);
  bool isForcedNaturalEnd ();
  virtual bool setOutWindow (SDLWindow* windowId);

  // Application player only.
  virtual void setCurrentScope (arg_unused (const string &scopeId)){};
};

struct notify
{
  IPlayerListener *listener;
  short code;
  string param;
  short type;
};

GINGA_PLAYER_END

#endif /* PLAYER_H */
