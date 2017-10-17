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
public:
  NclEvent (GingaInternal *, EventType, ExecutionObject *, Anchor *);
  virtual ~NclEvent ();

  virtual bool start ();
  virtual bool stop ();
  bool pause ();
  bool resume ();
  bool abort ();
  void addListener (INclEventListener *listener);

  EventType getType ();
  ExecutionObject *getObject ();
  Anchor *getAnchor ();

  EventState getState ();

protected:
  GingaInternal *_ginga;        // ginga handle
  Scheduler *_scheduler;        // scheduler

  EventType _type;              // event type
  ExecutionObject *_object;     // target object
  Anchor *_anchor;              // target anchor

  EventState _state;            // current state
  EventState _previousState;    // previous state

  set<INclEventListener *> _listeners;

  EventStateTransition getTransition (EventState);
  bool changeState (EventState, EventStateTransition);
};

class INclEventListener
{
public:
  virtual void eventStateChanged (NclEvent *,
                                  EventStateTransition,
                                  EventState) = 0;
};

class PresentationEvent: public NclEvent
{
  PROPERTY_READONLY (GingaTime, _begin, getBegin)
  PROPERTY (GingaTime, _end, getEnd, setEnd)

public:
  PresentationEvent (GingaInternal *, ExecutionObject *, Area *);
  virtual ~PresentationEvent () {}
  GingaTime getDuration ();
};

class SelectionEvent : public NclEvent
{
public:
  SelectionEvent (GingaInternal *, ExecutionObject *,
                  Area *, const string &);
  virtual ~SelectionEvent ();
  string getKey ();
  virtual bool start () override;
private:
  string _key;
};

class AttributionEvent : public NclEvent
{
public:
  AttributionEvent (GingaInternal *,
                    ExecutionObject *,
                    Property *);

  virtual ~AttributionEvent ();
};

class SwitchEvent : public NclEvent, public INclEventListener
{
  PROPERTY_READONLY (string, _key, getKey)

private:
  NclEvent *_mappedEvent;

public:
  SwitchEvent (GingaInternal *,
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
