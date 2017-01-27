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

#ifndef _INCLPlayer_H_
#define _INCLPlayer_H_

#include "system/ITimeBaseProvider.h"
using namespace ::ginga::system;

#include "IPlayer.h"
#include "IPlayerListener.h"

typedef struct
{
  string baseId;
  string playerId;
  int devClass;
  int x;
  int y;
  int w;
  int h;
  bool enableGfx;
  string parentDocId;
  string nodeId;
  string docId;
  double transparency;
  void *focusManager;
  void *privateBaseManager;
  bool enableMulticast;
  ::ginga::player::IPlayerListener *editListener;
} NclPlayerData;

GINGA_PLAYER_BEGIN

class INCLPlayer : public IPlayer
{
public:
  static const int DEEPEST_LEVEL = -1;

  virtual ~INCLPlayer (){};

  virtual void setMirrorSrc (IPlayer *mirrorSrc) = 0;
  virtual void printGingaWindows () = 0;
  virtual set<string> *createPortIdList () = 0;
  virtual short getMappedInterfaceType (string portId) = 0;

  virtual void setMrl (string mrl, bool visible = true) = 0;
  virtual void reset () = 0;
  virtual void rebase () = 0;
  virtual void setTimeBaseProvider (ITimeBaseProvider *timeBaseProvider)
      = 0;

  virtual void setParentLayout (void *layout) = 0;
  virtual string getScreenShot () = 0;
  virtual void *setCurrentDocument (string fName) = 0;
  virtual void setDepthLevel (int level) = 0;
  virtual int getDepthLevel () = 0;
  virtual bool editingCommand (string commandTag, string editingCommand)
      = 0;

  virtual void setNotifyContentUpdate (bool notify) = 0;
  virtual void addListener (IPlayerListener *listener) = 0;
  virtual void removeListener (IPlayerListener *listener) = 0;
  virtual void notifyPlayerListeners (short code, string paremeter,
                                      short type, string value)
      = 0;

  virtual void setSurface (SDLSurface* surface) = 0;
  virtual SDLSurface* getSurface () = 0;
  virtual int64_t
  getVPts ()
  {
    return 0;
  };
  virtual double getMediaTime () = 0;
  virtual double getTotalMediaTime () = 0;
  virtual void setMediaTime (double newTime) = 0;
  virtual bool setKeyHandler (bool isHandler) = 0;
  virtual void setScope (const string &scope, short type, double begin = -1,
                         double end = -1, double outTransDur = -1)
      = 0;

  virtual bool play () = 0;
  virtual void stop () = 0;
  virtual void abort () = 0;
  virtual void pause () = 0;
  virtual void resume () = 0;

  virtual string getPropertyValue (string const &name) = 0;
  virtual void setPropertyValue (const string &name, const string &value) = 0;

  virtual void setReferenceTimePlayer (IPlayer *player) = 0;
  virtual void addTimeReferPlayer (IPlayer *referPlayer) = 0;
  virtual void removeTimeReferPlayer (IPlayer *referPlayer) = 0;
  virtual void notifyReferPlayers (int transition) = 0;
  virtual void timebaseObjectTransitionCallback (int transition) = 0;
  virtual void setTimeBasePlayer (IPlayer *timeBasePlayer) = 0;
  virtual bool hasPresented () = 0;
  virtual void setPresented (bool presented) = 0;
  virtual bool isVisible () = 0;
  virtual void setVisible (bool visible) = 0;
  virtual bool immediatelyStart () = 0;
  virtual void setImmediatelyStart (bool immediatelyStartVal) = 0;
  virtual void forceNaturalEnd (bool forceIt) = 0;
  virtual bool isForcedNaturalEnd () = 0;
  virtual bool setOutWindow (SDLWindow* windowId) = 0;

  /*Exclusive for ChannelPlayer*/
  virtual IPlayer *getSelectedPlayer () = 0;
  virtual void setPlayerMap (map<string, IPlayer *> *objs) = 0;
  virtual map<string, IPlayer *> *getPlayerMap () = 0;
  virtual IPlayer *getPlayer (string objectId) = 0;
  virtual void select (IPlayer *selObject) = 0;

  /*Exclusive for Application Players*/
  virtual void setCurrentScope (string scopeId) = 0;
  virtual void flip () = 0;

  virtual string getDepUris (vector<string> *uris, int targetDev = 0) = 0;

  virtual void timeShift (string direction) = 0;
};

GINGA_PLAYER_END

#endif //_INCLPlayer_H_
