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

GINGA_FORMATTER_BEGIN

class INclEventListener;
class ExecutionObject;

class NclFormatterEvent
{
public:
  NclFormatterEvent (const string &id, ExecutionObject *);
  virtual ~NclFormatterEvent ();

  static bool hasInstance (NclFormatterEvent *event, bool remove);

  bool instanceOf (const string &s);

  static bool hasNcmId (NclFormatterEvent *event, const string &anchorId);

  void setEventType (short eventType);
  virtual short getEventType ();
  void setId (const string &id);
  void addEventListener (INclEventListener *listener);
  bool containsEventListener (INclEventListener *listener);
  void removeEventListener (INclEventListener *listener);

  bool abort ();
  virtual bool start ();
  virtual bool stop ();
  bool pause ();
  bool resume ();
  void setCurrentState (short newState);

  short getCurrentState ();
  short getPreviousState ();
  static short getTransistion (short previousState, short newState);

  ExecutionObject *getExecutionObject ();
  void setExecutionObject (ExecutionObject *object);
  string getId ();
  int getOccurrences ();
  static string getStateName (short state);

protected:
  string id;
  short currentState;
  short previousState;
  int occurrences;
  ExecutionObject *executionObject;
  set<INclEventListener *> listeners;
  set<string> typeSet;
  bool deleting;
  short eventType;

  static set<NclFormatterEvent *> instances;
  static bool init;

  static bool removeInstance (NclFormatterEvent *event);

  short getNewState (short transition);
  short getTransition (short newState);

  bool changeState (short newState, short transition);

private:
  virtual void destroyListeners ();
  static void addInstance (NclFormatterEvent *event);
};

GINGA_FORMATTER_END

#endif //_FORMATTEREVENT_H_
