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

#ifndef _FORMATTEREVENT_H_
#define _FORMATTEREVENT_H_

#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "INclEventListener.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_BEGIN

class NclFormatterEvent
{
private:
  static const short ST_ABORTED = 50;

protected:
  string id;
  short currentState;
  short previousState;
  long occurrences;
  void *executionObject;
  set<INclEventListener *> coreListeners;
  set<INclEventListener *> linksListeners;
  set<INclEventListener *> objectsListeners;
  set<string> typeSet;
  bool deleting;
  short eventType;
  pthread_mutex_t mutex;

  static set<NclFormatterEvent *> instances;
  static bool init;
  static pthread_mutex_t iMutex;

public:
  NclFormatterEvent (string id, void *executionObject);
  virtual ~NclFormatterEvent ();

private:
  virtual void destroyListeners ();

public:
  static bool hasInstance (NclFormatterEvent *event, bool remove);

private:
  static void addInstance (NclFormatterEvent *event);

protected:
  static bool removeInstance (NclFormatterEvent *event);

public:
  bool instanceOf (string s);

  static bool hasNcmId (NclFormatterEvent *event, string anchorId);

  void setEventType (short eventType);
  virtual short getEventType ();
  void setId (string id);
  void addEventListener (INclEventListener *listener);
  bool containsEventListener (INclEventListener *listener);
  void removeEventListener (INclEventListener *listener);

protected:
  short getNewState (short transition);
  short getTransition (short newState);

public:
  bool abort ();
  virtual bool start ();
  virtual bool stop ();
  bool pause ();
  bool resume ();
  void setCurrentState (short newState);

protected:
  bool changeState (short newState, short transition);

public:
  short getCurrentState ();
  short getPreviousState ();
  static short getTransistion (short previousState, short newState);

  void *getExecutionObject ();
  void setExecutionObject (void *object);
  string getId ();
  long getOccurrences ();
  static string getStateName (short state);
};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_END
#endif //_FORMATTEREVENT_H_
