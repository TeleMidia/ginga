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

#ifndef _FORMATTERPLAYER_H_
#define _FORMATTERPLAYER_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "mb/LocalScreenManager.h"
#include "mb/IInputEventListener.h"
#include "mb/SDLSurface.h"
using namespace ::ginga::mb;

#include "player/IPlayer.h"
using namespace ::ginga::player;

#include "ncl/Content.h"
#include "ncl/ContentNode.h"
#include "ncl/NodeEntity.h"
#include "ncl/ReferenceContent.h"
using namespace ::ginga::ncl;

#include "ncl/LambdaAnchor.h"
#include "ncl/IntervalAnchor.h"
using namespace ::ginga::ncl;

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "gingancl/model/CompositeExecutionObject.h"
#include "gingancl/model/ExecutionObject.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::components;

#include "gingancl/model/AttributionEvent.h"
#include "gingancl/model/FormatterEvent.h"
#include "gingancl/model/PresentationEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "IAdapterPlayer.h"
#include "NominalEventMonitor.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_BEGIN

class AdapterFormatterPlayer : public IAdapterPlayer,
                               public IPlayerListener,
                               public IAttributeValueMaintainer,
                               public IInputEventListener
{

protected:
  static LocalScreenManager *dm;
  static double eventTS;
  InputManager *im;

  void *manager;
  NominalEventMonitor *anchorMonitor;
  set<string> typeSet;
  ExecutionObject *object;
  IPlayer *player;
  IPlayer *mirrorSrc;
  string playerCompName;
  string mrl;
  int objectDevice;
  GingaScreenID myScreen;

private:
  double outTransDur;
  double outTransTime;
  bool isLocked;
  pthread_mutex_t objectMutex;

public:
  AdapterFormatterPlayer ();
  virtual ~AdapterFormatterPlayer ();

  virtual void setAdapterManager (void *manager);

  bool instanceOf (string s);
  virtual void setNclEditListener (IPlayerListener *listener){};
  void setOutputWindow (GingaWindowID windowId);

protected:
  virtual void rebase ();
  virtual void createPlayer ();

public:
  int getObjectDevice ();
  virtual bool hasPrepared ();
  bool setKeyHandler (bool isHandler);

protected:
  double prepareProperties (ExecutionObject *obj);
  void updatePlayerProperties (ExecutionObject *obj);

public:
  virtual bool prepare (ExecutionObject *object, FormatterEvent *mainEvent);

protected:
  void prepare ();
  virtual void prepareScope (double offset = -1);

private:
  double getOutTransDur ();

public:
  double getOutTransTime ();
  void checkAnchorMonitor ();

  static void printAction (string action, ExecutionObject *object);

  static void printAction (string command);

public:
  virtual bool start ();
  virtual bool stop ();
  virtual bool pause ();
  virtual bool resume ();
  virtual bool abort ();
  virtual void naturalEnd ();

private:
  bool checkRepeat (PresentationEvent *mainEvent);

public:
  virtual bool unprepare ();
  virtual bool setPropertyValue (AttributionEvent *event, string value);

  void setPropertyValue (string name, string value);

  string getPropertyValue (void *event);
  virtual string getPropertyValue (string name);

  double getObjectExpectedDuration ();
  void updateObjectExpectedDuration ();
  double getMediaTime ();
  IPlayer *getPlayer ();
  void setTimeBasePlayer (AdapterFormatterPlayer *timeBasePlayer);
  virtual void updateStatus (short code, string parameter = "",
                             short type = 10, string value = "");

  bool userEventReceived (SDLInputEvent *ev);

private:
  void setVisible (bool visible);

protected:
  bool lockObject ();
  bool unlockObject ();

public:
  virtual void flip (){};
  virtual void timeShift (string direction);
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_END
using namespace ::br::pucrio::telemidia::ginga::ncl::adapters;

#endif //_FORMATTERPLAYER_H_
