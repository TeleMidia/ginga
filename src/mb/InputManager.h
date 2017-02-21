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

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "EventBuffer.h"
#include "InputEvent.h"
#include "ICmdEventListener.h"
#include "IInputEventListener.h"
#include "IMotionEventListener.h"

typedef struct lockedLitenerAction
{
  ::ginga::mb::IInputEventListener *l;
  bool isAdd;
  set<int> *events;
} LockedAction;

GINGA_MB_BEGIN

class InputManager : public Thread
{
protected:
  map<IInputEventListener *, set<int> *> eventListeners;
  vector<LockedAction *> actionsToInpListeners;
  set<IInputEventListener *> applicationListeners;
  vector<LockedAction *> actionsToAppListeners;

  set<IMotionEventListener *> motionListeners;

  ICmdEventListener *cmdListener;

  pthread_mutex_t mlMutex;

  bool running;
  bool notifying;
  bool notifyingApp;
  EventBuffer *eventBuffer;
  double lastEventTime;
  double imperativeIntervalTime;
  double declarativeIntervalTime;

  int currentXAxis;
  int currentYAxis;
  int currentZAxis;
  int maxX;
  int maxY;

  pthread_mutex_t actAppMutex;
  pthread_mutex_t actInpMutex;

  pthread_mutex_t appMutex;

public:
  InputManager ();
  ~InputManager ();

protected:
  void initializeInputIntervalTime ();

  void release ();

public:
  void addMotionEventListener (IMotionEventListener *listener);
  void removeMotionEventListener (IMotionEventListener *listener);

protected:
  virtual void notifyMotionListeners (int x, int y, int z);

public:
  void addInputEventListener (IInputEventListener *listener,
                              set<int> *events = NULL);

  void removeInputEventListener (IInputEventListener *listener);

public:
  void setCommandEventListener (ICmdEventListener *listener);

protected:
  void performInputLockedActions ();
  void performApplicationLockedActions ();
  virtual bool dispatchEvent (InputEvent *keyEvent);
  virtual bool dispatchApplicationEvent (InputEvent *keyEvent);

public:
  void addApplicationInputEventListener (IInputEventListener *listener);
  void removeApplicationInputEventListener (IInputEventListener *listener);

  void postInputEvent (InputEvent *event);
  void postInputEvent (Key::KeyCode keyCode);
  void postCommand (const string &cmd, const string &args);

  void setAxisValues (int x, int y, int z);
  void setAxisBoundaries (int x, int y, int z);
  int getCurrentXAxisValue ();
  int getCurrentYAxisValue ();

  EventBuffer *getEventBuffer ();

protected:
  virtual void run ();
  void handleInputEvent (InputEvent *inputEvent, int &pLastCode,
                         int &lastCode, double &pTimeStamp,
                         double &timeStamp, int &mouseX, int &mouseY);
};

GINGA_MB_END

#endif /*INPUTMANAGER_H_*/
