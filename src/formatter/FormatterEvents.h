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

#define PROPERTY_READONLY(type,name,getfunc) \
  protected: type name; \
  public: type getfunc () const { return this->name; }

#define PROPERTY(type,name,getfunc,setfunc) \
  PROPERTY_READONLY (type,name,getfunc) \
  public: void setfunc (type value) {this->name = value;}\

class NclFormatterEvent
{
  PROPERTY (EventType, _type, getType, setType)
  PROPERTY_READONLY (string, _id, getId)
  PROPERTY_READONLY (int, _occurrences, getOccurrences)
  PROPERTY (ExecutionObject *, _exeObj, getExecutionObject, setExecutionObject)
  PROPERTY_READONLY (EventState, _state, getCurrentState)
  PROPERTY_READONLY (EventState, _previousState, getPreviousState)

public:
  NclFormatterEvent (const string &id, ExecutionObject *exeObj);
  virtual ~NclFormatterEvent ();

  void setState (EventState newState);

  virtual bool start ();
  virtual bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();

  void addListener (INclEventListener *listener);
  void removeListener (INclEventListener *listener);

  bool instanceOf (const string &);
  static bool hasInstance (NclFormatterEvent *evt, bool remove);
  static bool hasNcmId (NclFormatterEvent *evt, const string &anchorId);

protected:
  set<INclEventListener *> _listeners;
  set<string> _typeSet;

  EventStateTransition getTransition (EventState newState);
  bool changeState (EventState newState, EventStateTransition transition);

  static set<NclFormatterEvent *> _instances;
  static bool removeInstance (NclFormatterEvent *evt);

private:
  static void addInstance (NclFormatterEvent *event);
};

class NclAnchorEvent : public NclFormatterEvent
{
  PROPERTY_READONLY (ContentAnchor *, _anchor, getAnchor)

public:
  NclAnchorEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~NclAnchorEvent () {}
};

class NclPresentationEvent : public NclAnchorEvent
{
  PROPERTY_READONLY (GingaTime, _begin, getBegin)
  PROPERTY (GingaTime, _end, getEnd, setEnd)
  PROPERTY_READONLY (GingaTime, _repetitionInterval, getRepetitionInterval)

public:
  NclPresentationEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~NclPresentationEvent () {}

  bool stop ();

  GingaTime getDuration ();
  int getRepetitions ();
  void setRepetitionSettings (int repetitions, GingaTime repetitionInterval);
  void incrementOccurrences ();

private:
  int _numPresentations;
};

class NclSelectionEvent : public NclAnchorEvent
{
  PROPERTY (string, _selCode, getSelectionCode, setSelectionCode)

public:
  NclSelectionEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~NclSelectionEvent () {}

  bool start ();
};

class NclAttributionEvent : public NclFormatterEvent
{
  PROPERTY (INclAttributeValueMaintainer *, _valueMaintainer,
            getValueMaintainer, setValueMaintainer)

  PROPERTY_READONLY (PropertyAnchor *, _anchor, getAnchor)

public:
  NclAttributionEvent (const string &id, ExecutionObject *,
                       PropertyAnchor *,
                       Settings *);

  virtual ~NclAttributionEvent ();
  string getCurrentValue ();
  bool setValue (const string &newValue);
  void setImplicitRefAssessmentEvent (const string &roleId,
                                      NclFormatterEvent *event);

  NclFormatterEvent *getImplicitRefAssessmentEvent (const string &roleId);

protected:
  map<string, NclFormatterEvent *> _assessments;
  Settings *_settings;

private:
  bool _settingsNode;
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
      NclFormatterEvent *evt,
      EventStateTransition transition,
      EventState previousState) override;
};

GINGA_FORMATTER_END

#endif //_FORMATTEREVENT_H_
