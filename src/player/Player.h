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

#include "system/GingaLocatorFactory.h"
#include "system/SystemCompat.h"
#include "system/Thread.h"
using namespace ::ginga::system;

#include "mb/InputManager.h"
#include "mb/IInputEventListener.h"
#include "mb/Key.h"
using namespace ::ginga::mb;

#include "IPlayer.h"

#ifndef HAVE_CLOCKTIME
#define HAVE_CLOCKTIME 1
#endif

GINGA_PLAYER_BEGIN


enum PLAYER_STATUS{
  SLEEPING,
  OCCURRING,
  PAUSED
};

typedef struct LockedPlayerLitenerAction
{
  ::ginga::player::IPlayerListener *l;
  bool isAdd;
} LockedPlayerListener;

typedef struct PendingNotification
{
  short code;
  string parameter;
  short type;
  string value;

  set<IPlayerListener *> *clone;
} PendingNotification;
class Player : public IPlayer
{
private:
  pthread_mutex_t listM;
  pthread_mutex_t lockedListM;
  pthread_mutex_t referM;
  pthread_mutex_t pnMutex;

  bool notifying;

  map<string, string> properties;
  set<IPlayerListener *> listeners;
  vector<LockedPlayerListener *> lockedListeners;
  vector<PendingNotification *> pendingNotifications;

protected:
  PLAYER_STATUS status;

  string mrl;
  SDLSurface *surface;
  SDLWindow *outputWindow;
  double initTime, elapsedTime, elapsedPause, pauseTime;
  set<IPlayer *> referredPlayers;
  IPlayer *timeBasePlayer;
  bool presented;
  bool visible;
  bool immediatelyStartVar;
  bool forcedNaturalEnd;
  bool notifyContentUpdate;
  string scope;
  short scopeType;
  double scopeInitTime;
  double scopeEndTime;
  double outTransTime;
  IPlayer *mirrorSrc;
  set<IPlayer *> mirrors;

public:
  Player (const string &mrl);
  virtual ~Player ();
  void setMirrorSrc (IPlayer *mirrorSrc);

private:
  void addMirror (IPlayer *mirror);
  bool removeMirror (IPlayer *mirror);

public:
  virtual void flip (){};
  virtual void setMrl (const string &mrl, bool visible = true);
  virtual void reset (){};
  virtual void rebase (){};
  virtual void setNotifyContentUpdate (bool notify);
  virtual void addListener (IPlayerListener *listener);
  void removeListener (IPlayerListener *listener);

private:
  void performLockedListenersRequest ();

public:
  void notifyPlayerListeners (short code,
                              const string &parameter = "",
                              short type = TYPE_PRESENTATION,
                              const string &value = "");

private:
  static void *detachedNotifier (void *ptr);
  static void ntsNotifyPlayerListeners (set<IPlayerListener *> *list,
                                        short code,
                                        const string &parameter,
                                        short type,
                                        const string &value);

public:
  virtual void setSurface (SDLSurface* surface);
  virtual SDLSurface* getSurface ();

  virtual void setMediaTime (double newTime);
  virtual int64_t
  getVPts ()
  {
    clog << "Player::getVPts return 0" << endl;
    return 0;
  };

#if HAVE_CLOCKTIME
  double getMediaTime ();
#else
  virtual double getMediaTime ();
#endif

  virtual double getTotalMediaTime ();

  virtual bool setKeyHandler (bool isHandler);
  virtual void setScope (const string &scope, short type = TYPE_PRESENTATION,
                         double begin = -1, double end = -1,
                         double outTransDur = -1);

private:
  void mirrorIt (Player *mirrorSrc, Player *mirror);
  void checkMirrors ();

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
  virtual bool hasPresented ();
  void setPresented (bool presented);
  bool isVisible ();
  void setVisible (bool visible);
  bool immediatelyStart ();
  void setImmediatelyStart (bool immediatelyStartVal);

protected:
  void checkScopeTime ();

private:
  static void *scopeTimeHandler (void *ptr);

public:
  void forceNaturalEnd (bool forceIt);
  bool isForcedNaturalEnd ();
  virtual bool setOutWindow (SDLWindow* windowId);

  // Channel player only.
  virtual IPlayer *
  getSelectedPlayer ()
  {
    return NULL;
  };
  virtual void setPlayerMap (arg_unused (map<string, IPlayer *> *objs)){};
  virtual map<string, IPlayer *> *
  getPlayerMap ()
  {
    return NULL;
  };
  virtual IPlayer *
  getPlayer (arg_unused (const string &objectId))
  {
    return NULL;
  };
  virtual void select (arg_unused (IPlayer *selObject)){};

  // Application player only.
  virtual void setCurrentScope (arg_unused (const string &scopeId)){};

  virtual void timeShift (arg_unused (const string &direction)){};
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
