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

#include "aux-ginga.h"

GINGA_NAMESPACE_BEGIN

class FormatterObject;
class FormatterEvent
{
public:
  enum Type
    {
     SELECTION = 0,
     PRESENTATION,
     ATTRIBUTION,
    };

  enum State
    {
     SLEEPING = 0,
     OCCURRING,
     PAUSED
    };

  enum Transition
    {
     START = 0,
     PAUSE,
     RESUME,
     STOP,
     ABORT
    };

  FormatterEvent (FormatterEvent::Type, FormatterObject *, const string &id);
  ~FormatterEvent ();

  FormatterEvent::Type getType ();
  FormatterObject *getObject ();
  string getId ();
  FormatterEvent::State getState ();

  bool isLambda ();
  void getInterval (GingaTime *, GingaTime *);
  void setInterval (GingaTime, GingaTime);

  bool getParameter (const string &, string *);
  bool setParameter (const string &, const string &);

  bool transition (FormatterEvent::Transition);
  void reset ();

public:
  static string getEventTypeAsString (FormatterEvent::Type);
  static string getEventStateAsString (FormatterEvent::State);
  static string getEventTransitionAsString (FormatterEvent::Transition);

private:
  FormatterEvent::Type _type;      // event type
  FormatterObject *_object;        // target object
  string _id;                      // event id
  FormatterEvent::State _state;    // event state
  GingaTime _begin;                // begin-time
  GingaTime _end;                  // end-time
  map<string, string> _parameters; // parameters
};

GINGA_NAMESPACE_END

#endif // FORMATTER_EVENT_H
