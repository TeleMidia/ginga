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

#include "GingaInternal.h"

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
  NclEvent (GingaInternal *, const string &, ExecutionObject *);
  virtual ~NclEvent ();
  void setState (EventState);
  virtual bool start ();
  virtual bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();
  void addListener (INclEventListener *listener);

protected:
  GingaInternal *_ginga;        // ginga handle
  Scheduler *_scheduler;        // scheduler

  set<INclEventListener *> _listeners;

  EventStateTransition getTransition (EventState newState);
  bool changeState (EventState newState, EventStateTransition transition);
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
  PROPERTY_READONLY (Area *, _anchor, getAnchor)

public:
  AnchorEvent (GingaInternal *, const string &, ExecutionObject *, Area *);
  virtual ~AnchorEvent () {}
};

class PresentationEvent : public AnchorEvent
{
  PROPERTY_READONLY (GingaTime, _begin, getBegin)
  PROPERTY (GingaTime, _end, getEnd, setEnd)

public:
  PresentationEvent (GingaInternal *, const string &,
                     ExecutionObject *, Area *);
  virtual ~PresentationEvent () {}
  virtual bool stop () override;
  GingaTime getDuration ();

private:
  int _numPresentations;
};

class SelectionEvent : public AnchorEvent
{
  PROPERTY (string, _selCode, getSelectionCode, setSelectionCode)

public:
  SelectionEvent (GingaInternal *, const string &, ExecutionObject *, Area *);
  virtual ~SelectionEvent () {}

  virtual bool start () override;
};

class AttributionEvent : public NclEvent
{
  PROPERTY (Player *, _player, getPlayer, setPlayer)
  PROPERTY_READONLY (Property *, _anchor, getAnchor)

public:
  AttributionEvent (GingaInternal *,
                    const string &,
                    ExecutionObject *,
                    Property *);

  virtual ~AttributionEvent ();
  string getCurrentValue ();
  bool setValue (const string &newValue);
  void setImplicitRefAssessmentEvent (const string &roleId,
                                      NclEvent *event);

  NclEvent *getImplicitRefAssessmentEvent (const string &roleId);
  string solveImplicitRefAssessment (const string &val);

protected:
  map<string, NclEvent *> _assessments;
};

class SwitchEvent : public NclEvent, public INclEventListener
{
  PROPERTY_READONLY (Anchor*, _interface, getInterface)
  PROPERTY_READONLY (string, _key, getKey)

private:
  NclEvent *_mappedEvent;

public:
  SwitchEvent (GingaInternal *,
               const string &,
               ExecutionObject *,
               Anchor *,
               EventType ,
               const string &);

  virtual ~SwitchEvent ();

  void setMappedEvent (NclEvent *evt);
  NclEvent *getMappedEvent () { return this->_mappedEvent; }

  virtual void eventStateChanged (
      NclEvent *evt,
      EventStateTransition trans,
      EventState prevState) override;
};

GINGA_FORMATTER_END

#endif // NCL_EVENTS_H
