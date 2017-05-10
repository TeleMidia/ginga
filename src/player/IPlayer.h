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

#ifndef IPLAYER_H_
#define IPLAYER_H_

#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "IPlayerListener.h"

GINGA_PLAYER_BEGIN

class IPlayer
{
public:
  enum NotifyCode : short
    {
      PL_NOTIFY_START = 1,
      PL_NOTIFY_PAUSE,
      PL_NOTIFY_RESUME,
      PL_NOTIFY_STOP,
      PL_NOTIFY_ABORT,
      PL_NOTIFY_NCLEDIT,
      PL_NOTIFY_UPDATECONTENT,
      PL_NOTIFY_OUTTRANS,

      TYPE_PRESENTATION,
      TYPE_ATTRIBUTION,
      TYPE_SIGNAL,
      TYPE_SELECTION
    };

  virtual ~IPlayer (){};

  virtual void setMrl (const string &mrl, bool visible = true) = 0;
  virtual void setNotifyContentUpdate (bool notify) = 0;
  virtual void addListener (IPlayerListener *listener) = 0;
  virtual void removeListener (IPlayerListener *listener) = 0;
  virtual void notifyPlayerListeners (short code,
                                      const string &parameter,
                                      short type,
                                      const string &value)
      = 0;

  virtual int64_t getVPts () = 0;
  virtual guint32 getMediaTime () = 0;
  virtual double getTotalMediaTime () = 0;
  virtual void setMediaTime (guint32 newTime) = 0;
  virtual void setScope (const string &scope, short type, double begin = -1,
                         double end = -1, double outTransDur = -1)
      = 0;

  virtual bool play () = 0;
  virtual void stop () = 0;
  virtual void abort () = 0;
  virtual void pause () = 0;
  virtual void resume () = 0;

  virtual string getPropertyValue (const string &name) = 0;
  virtual void setPropertyValue (const string &name, const string &value) = 0;

  virtual void setReferenceTimePlayer (IPlayer *player) = 0;
  virtual void addTimeReferPlayer (IPlayer *referPlayer) = 0;
  virtual void removeTimeReferPlayer (IPlayer *referPlayer) = 0;
  virtual void notifyReferPlayers (int transition) = 0;
  virtual void timebaseObjectTransitionCallback (int transition) = 0;
  virtual void setTimeBasePlayer (IPlayer *timeBasePlayer) = 0;
  virtual bool isVisible () = 0;
  virtual void setVisible (bool visible) = 0;
  virtual bool immediatelyStart () = 0;
  virtual void setImmediatelyStart (bool immediatelyStartVal) = 0;
  virtual void forceNaturalEnd (bool forceIt) = 0;
  virtual bool isForcedNaturalEnd () = 0;
  virtual bool setOutWindow (SDLWindow* windowId) = 0;

  /*Exclusive for Application Players*/
  virtual void setCurrentScope (const string &scopeId) = 0;
};

GINGA_PLAYER_END

#endif /*IPLAYER_H_*/
