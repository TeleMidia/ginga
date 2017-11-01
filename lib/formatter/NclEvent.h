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

#ifndef NCL_EVENT_H
#define NCL_EVENT_H

#include "GingaInternal.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class NclEvent;
class INclEventListener
{
public:
  virtual void eventStateChanged (NclEvent *, EventStateTransition) = 0;
};

class ExecutionObject;
class NclEvent
{
public:
  NclEvent (GingaInternal *, EventType, ExecutionObject *,
            Anchor *);
  virtual ~NclEvent ();

  EventType getType ();
  ExecutionObject *getObject ();
  Anchor *getAnchor ();
  EventState getState ();
  bool getInterval (GingaTime *, GingaTime *); // presentation

  const vector<INclEventListener *> *getListeners ();
  void addListener (INclEventListener *);
  string getParameter (const string &);
  void setParameter (const string &, const string &);

  bool transition (EventStateTransition);

private:
  GingaInternal *_ginga;                  // ginga handle
  Scheduler *_scheduler;                  // scheduler
  EventType _type;                        // event type
  ExecutionObject *_object;               // target object
  Anchor *_anchor;                        // target anchor
  EventState _state;                      // event state
  vector<INclEventListener *> _listeners; // event listeners
  map<string,string> _params;             // parameters
};

GINGA_FORMATTER_END

#endif // NCL_EVENT_H
