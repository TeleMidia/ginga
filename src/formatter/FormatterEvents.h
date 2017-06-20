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

#include "ncl/ContentAnchor.h"
#include "ncl/EventUtil.h"
using namespace ::ginga::ncl;

#include "INclEventListener.h"
#include "INclAttributeValueMaintainer.h"
#include "Settings.h"

#include "ncl/PropertyAnchor.h"

GINGA_FORMATTER_BEGIN

class INclEventListener;
class ExecutionObject;

class NclFormatterEvent
{
public:
  NclFormatterEvent (const string &id, ExecutionObject *exeObj);
  virtual ~NclFormatterEvent ();

  static bool hasInstance (NclFormatterEvent *evt, bool remove);

  bool instanceOf (const string &);

  static bool hasNcmId (NclFormatterEvent *evt, const string &anchorId);

  void setType (EventType evtType);
  EventType getType ();

  void addListener (INclEventListener *listener);
  void removeListener (INclEventListener *listener);

  void setCurrentState (EventState newState);

  EventState getCurrentState ();
  EventState getPreviousState ();

  virtual bool start ();
  virtual bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();

  ExecutionObject *getExecutionObject ();
  void setExecutionObject (ExecutionObject *exeObj);

  string getId ();
  int getOccurrences ();

protected:
  string id;
  EventState _currentState;
  EventState _previousState;
  int _occurrences;
  ExecutionObject *_exeObj;
  set<INclEventListener *> _listeners;
  set<string> _typeSet;
  bool _deleting;
  EventType _eventType;

  static set<NclFormatterEvent *> _instances;
  static bool _init;

  static bool removeInstance (NclFormatterEvent *evt);

  EventState getNewState (EventStateTransition transition);
  EventStateTransition getTransition (EventState newState);
  bool changeState (EventState newState, EventStateTransition transition);

private:
  virtual void destroyListeners ();
  static void addInstance (NclFormatterEvent *event);
};

class NclAnchorEvent : public NclFormatterEvent
{
public:
  NclAnchorEvent (const string &, ExecutionObject *, ContentAnchor *);

  virtual ~NclAnchorEvent ();

  ContentAnchor *getAnchor ();
  virtual bool
  start ()
  {
    return NclFormatterEvent::start ();
  }
  virtual bool
  stop ()
  {
    return NclFormatterEvent::stop ();
  }

protected:
  ContentAnchor *_anchor;
};

class NclPresentationEvent : public NclAnchorEvent
{
public:
  NclPresentationEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~NclPresentationEvent ();

  bool stop ();

  GingaTime getDuration ();
  GingaTime getRepetitionInterval ();
  int getRepetitions ();
  void setEnd (GingaTime e);
  void setRepetitionSettings (int repetitions, GingaTime repetitionInterval);
  GingaTime getBegin ();
  GingaTime getEnd ();
  void incrementOccurrences ();

private:
  GingaTime begin;
  GingaTime end;
  int numPresentations;
  GingaTime repetitionInterval;
};

class NclSelectionEvent : public NclAnchorEvent
{
public:
  NclSelectionEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~NclSelectionEvent ();
  bool start ();
  const string getSelectionCode ();
  void setSelectionCode (const string &codeStr);

private:
  string selectionCode;
};

class NclAttributionEvent : public NclFormatterEvent
{
private:
  bool settingNode;

protected:
  PropertyAnchor *anchor;
  INclAttributeValueMaintainer *valueMaintainer;
  map<string, NclFormatterEvent *> assessments;
  Settings *settings;

public:
  NclAttributionEvent (const string &id, ExecutionObject *,
                       PropertyAnchor *,
                       Settings *);

  virtual ~NclAttributionEvent ();
  PropertyAnchor *getAnchor ();
  string getCurrentValue ();
  bool setValue (const string &newValue);
  void setValueMaintainer (INclAttributeValueMaintainer *valueMaintainer);
  INclAttributeValueMaintainer *getValueMaintainer ();
  void setImplicitRefAssessmentEvent (const string &roleId,
                                      NclFormatterEvent *event);

  NclFormatterEvent *getImplicitRefAssessmentEvent (const string &roleId);
};

class NclSwitchEvent : public NclFormatterEvent, public INclEventListener
{
private:
  InterfacePoint *interfacePoint;
  string key;
  NclFormatterEvent *mappedEvent;

public:
  NclSwitchEvent (const string &, ExecutionObject *, InterfacePoint *,
                  EventType, const string &);

  virtual ~NclSwitchEvent ();

  InterfacePoint *getInterfacePoint ();
  string getKey ();
  void setMappedEvent (NclFormatterEvent *event);
  NclFormatterEvent *getMappedEvent ();
  virtual void eventStateChanged (
      NclFormatterEvent *event,
      EventStateTransition transition,
      EventState _previousState) override;
};

GINGA_FORMATTER_END

#endif //_FORMATTEREVENT_H_
