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

#include "mb/IKeyInputEventListener.h"

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

#include "NclCompositeExecutionObject.h"
#include "NclExecutionObject.h"

#include "NclAttributionEvent.h"
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"

GINGA_FORMATTER_BEGIN

class AdapterPlayerManager;

class AdapterFormatterPlayer :
    public IPlayerListener,
    public INclAttributeValueMaintainer,
    public IKeyInputEventListener
{

public:
  AdapterFormatterPlayer ();
  virtual ~AdapterFormatterPlayer ();

  void setAdapterManager (AdapterPlayerManager *manager);

  void setOutputWindow (SDLWindow* windowId);

  virtual bool hasPrepared ();

  virtual bool prepare (NclExecutionObject *obj, NclFormatterEvent *mainEvent);

  void checkAnchorMonitor ();

  virtual bool start ();
  virtual bool stop ();
  virtual bool pause ();
  virtual bool resume ();
  virtual bool abort ();
  virtual void naturalEnd ();

  virtual bool unprepare ();

  virtual bool setPropertyValue (NclAttributionEvent *evt, const string &value);
  void setPropertyValue (const string &name, const string &value);

  string getPropertyValue (NclAttributionEvent *event);
  virtual string getPropertyValue (const string &name);

  double getObjectExpectedDuration ();
  void updateObjectExpectedDuration ();
  double getMediaTime ();
  IPlayer *getPlayer ();
  void setTimeBasePlayer (AdapterFormatterPlayer *timeBasePlayer);
  virtual void updateStatus (short code,
                             const string &parameter = "",
                             short type = 10,
                             const string &value = "");

  virtual void keyInputCallback (SDL_EventType evtType, SDL_Keycode key);

protected:
  static double _eventTS;

  AdapterPlayerManager *_manager;
  NclExecutionObject *_object;
  IPlayer *_player;
  string _mrl;

  virtual void createPlayer ();
  double prepareProperties (NclExecutionObject *obj);
  void updatePlayerProperties ();
  void prepare ();
  virtual void prepareScope (double offset = -1);
  bool lockObject ();
  bool unlockObject ();

private:
  double _outTransDur;
  double _outTransTime;
  bool _isLocked;
  pthread_mutex_t _objectMutex;

  double getOutTransDur ();
  bool checkRepeat (NclPresentationEvent *mainEvent);
  void setVisible (bool visible);
};

GINGA_FORMATTER_END

#endif //_FORMATTERPLAYER_H_
