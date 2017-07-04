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

#ifndef NCL_EVENTS_H
#define NCL_EVENTS_H

#include "Settings.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

class INclEventListener;
class ExecutionObject;

#define PROPERTY_READONLY(type,name,getfunc)\
  protected: type name;\
  public: type getfunc () const { return this->name; }

#define PROPERTY(type,name,getfunc,setfunc) \
  PROPERTY_READONLY (type,name,getfunc) \
  public: void setfunc (type value) {this->name = value;}\

class NclEvent
{
  PROPERTY (EventType, _type, getType, setType)
  PROPERTY_READONLY (string, _id, getId)
  PROPERTY_READONLY (int, _occurrences, getOccurrences)
  PROPERTY (ExecutionObject *, _exeObj, getExecutionObject, setExecutionObject)
  PROPERTY_READONLY (EventState, _state, getCurrentState)
  PROPERTY_READONLY (EventState, _previousState, getPreviousState)

public:
  NclEvent (const string &, ExecutionObject *);
  virtual ~NclEvent ();
  void setState (EventState newState);
  virtual bool start ();
  virtual bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();
  void addListener (INclEventListener *listener);
  void removeListener (INclEventListener *listener);
  static bool hasInstance (NclEvent *evt, bool remove);
  static bool hasNcmId (NclEvent *evt, const string &anchorId);

protected:
  set<INclEventListener *> _listeners;

  EventStateTransition getTransition (EventState newState);
  bool changeState (EventState newState, EventStateTransition transition);

  static set<NclEvent *> _instances;
  static bool removeInstance (NclEvent *evt);
};

class INclEventListener
{
public:
  virtual void eventStateChanged (NclEvent *,
                                  EventStateTransition,
                                  EventState) = 0;
};

class AnchorEvent : public NclEvent
{
  PROPERTY_READONLY (ContentAnchor *, _anchor, getAnchor)

public:
  AnchorEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~AnchorEvent () {}
};

class PresentationEvent : public AnchorEvent
{
  PROPERTY_READONLY (GingaTime, _begin, getBegin)
  PROPERTY (GingaTime, _end, getEnd, setEnd)
  PROPERTY_READONLY (GingaTime, _repetitionInterval, getRepetitionInterval)

public:
  PresentationEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~PresentationEvent () {}
  virtual bool stop () override;
  GingaTime getDuration ();
  int getRepetitions ();
  void setRepetitionSettings (int, GingaTime);
  void incOccurrences ();

private:
  int _numPresentations;
};

class SelectionEvent : public AnchorEvent
{
  PROPERTY (string, _selCode, getSelectionCode, setSelectionCode)

public:
  SelectionEvent (const string &, ExecutionObject *, ContentAnchor *);
  virtual ~SelectionEvent () {}

  virtual bool start () override;
};

class AttributionEvent : public NclEvent
{
  PROPERTY (Player *, _player, getPlayer, setPlayer)
  PROPERTY_READONLY (PropertyAnchor *, _anchor, getAnchor)

public:
  AttributionEvent (const string &id,
                    ExecutionObject *exeObj,
                    PropertyAnchor *anchor,
                    Settings *settings);

  virtual ~AttributionEvent ();
  string getCurrentValue ();
  bool setValue (const string &newValue);
  void setImplicitRefAssessmentEvent (const string &roleId,
                                      NclEvent *event);

  NclEvent *getImplicitRefAssessmentEvent (const string &roleId);
  string solveImplicitRefAssessment (const string &val);

protected:
  map<string, NclEvent *> _assessments;
  Settings *_settings;

private:
  bool _settingsNode;
};

class SwitchEvent : public NclEvent, public INclEventListener
{
private:
  InterfacePoint *_interface;
  string _key;
  NclEvent *_mappedEvent;

public:
  SwitchEvent (const string &id,
               ExecutionObject *exeObjSwitch,
               InterfacePoint *interface,
               EventType type,
               const string &key);

  virtual ~SwitchEvent ();

  InterfacePoint *getInterfacePoint () { return this->_interface; }
  string getKey () { return this->_key; }

  void setMappedEvent (NclEvent *evt);
  NclEvent *getMappedEvent () { return this->_mappedEvent; }

  virtual void eventStateChanged (
      NclEvent *evt,
      EventStateTransition trans,
      EventState prevState) override;
};

class EventTransition
{
  PROPERTY_READONLY (PresentationEvent *, _evt, getEvent)
  PROPERTY_READONLY (GingaTime, _time, getTime)

public:
  EventTransition (GingaTime time, PresentationEvent *evt);
  virtual ~EventTransition () {}
};

class EndEventTransition;

class BeginEventTransition : public EventTransition
{
  PROPERTY (EndEventTransition *, _endTrans, getEndTransition, setEndTransition)

public:
  BeginEventTransition (GingaTime time, PresentationEvent *evt);
};

class EndEventTransition : public EventTransition
{
  PROPERTY_READONLY (BeginEventTransition *, _beginTrans, getBeginTransition)

public:
  EndEventTransition (GingaTime t, PresentationEvent *evt,
                      BeginEventTransition *trans);
};

GINGA_FORMATTER_END

#endif // NCL_EVENTS_H
