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

#ifndef FORMATTER_EVENT_H
#define FORMATTER_EVENT_H

#include "GingaInternal.h"

#include "ncl/Ncl.h"
using namespace ::ginga::ncl;

GINGA_FORMATTER_BEGIN

class FormatterEvent;
class IFormatterEventListener
{
public:
  virtual void eventStateChanged (FormatterEvent *,
                                  EventStateTransition) = 0;
};

class ExecutionObject;
class FormatterEvent
{
public:
  FormatterEvent (GingaInternal *, EventType, ExecutionObject *,
                  Anchor *);
  virtual ~FormatterEvent ();

  EventType getType ();
  ExecutionObject *getObject ();
  Anchor *getAnchor ();
  EventState getState ();

  const vector<IFormatterEventListener *> *getListeners ();
  void addListener (IFormatterEventListener *);
  string getParameter (const string &);
  void setParameter (const string &, const string &);

  bool transition (EventStateTransition);

private:
  GingaInternal *_ginga;                        // ginga handle
  Scheduler *_scheduler;                        // scheduler
  EventType _type;                              // event type
  ExecutionObject *_object;                     // target object
  Anchor *_anchor;                              // target anchor
  EventState _state;                            // event state
  vector<IFormatterEventListener *> _listeners; // event listeners
  map<string,string> _params;                   // parameters
};

GINGA_FORMATTER_END

#endif // FORMATTER_EVENT_H
