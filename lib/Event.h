/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef EVENT_H
#define EVENT_H

#include "aux-ginga.h"
#include "Predicate.h"

GINGA_NAMESPACE_BEGIN

class Object;
class Event
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

  Event (Event::Type, Object *, const string &id);
  ~Event ();

  Event::Type getType ();
  Object *getObject ();
  string getId ();
  string getFullId ();
  Event::State getState ();
  string toString ();

  bool isLambda ();
  void getInterval (Time *, Time *);
  void setInterval (Time, Time);

  bool getParameter (const string &, string *);
  bool setParameter (const string &, const string &);

  bool transition (Event::Transition);
  void reset ();

public:
  static string getEventTypeAsString (Event::Type);
  static string getEventStateAsString (Event::State);
  static string getEventTransitionAsString (Event::Transition);

private:
  Event::Type _type;               // event type
  Object *_object;                 // target object
  string _id;                      // event id
  Event::State _state;             // event state
  Time _begin;                     // begin-time
  Time _end;                       // end-time
  map<string, string> _parameters; // parameters
};

typedef struct
{
  Event *event;                 // Target event
  Event::Transition transition; // Target transition
  Predicate *predicate;         // if appears in condition
  string value;                 // if attribution event
} Action;

GINGA_NAMESPACE_END

#endif // EVENT_H
