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

#ifndef GINGA_EVENT_H
#define GINGA_EVENT_H

#include "aux-ginga.h"
#include "Predicate.h"

GINGA_NAMESPACE_BEGIN

class Object;

/// Event in an NCL object.
///
/// Run-time representation of an NCL event state machine.
///
/// @see Object.
///
class Event
{
public:

  /// Possible types for NCL events.
  enum Type
  {
     ATTRIBUTION,               ///< Attribution of a property.
     PRESENTATION,              ///< Presentation of a time interval.
     SELECTION,                 ///< Key press/release.
  };

  /// Possible states for NCL events.
  enum State
  {
     OCCURRING,                 ///< The event is occurring.
     PAUSED,                    ///< The event is paused.
     SLEEPING,                  ///< The event is sleeping.
  };

  /// Possible transitions between states of an NCL event.
  enum Transition
  {
     ABORT,                     ///< `Occurring|Paused->Sleeping`.
     PAUSE,                     ///< `Occurring->Paused`.
     RESUME,                    ///< `Paused->Occurring`.
     START,                     ///< `Paused|Sleeping->Occurring`.
     STOP,                      ///< `Occurring|Paused->Sleeping`.
  };

  /// Creates a new event.
  ///
  /// @param object The container object.
  /// @param type The type of the new event.
  /// @param id The id of the new event (must no occur in \p object).
  /// @return The newly created event.
  ///
  /// @warning Do not call this constructor directly.  Use
  /// Document::createEvent() or Object::createEvent() to create events.
  ///
  Event (Object *object, Event::Type type, const string &id);

  /// Destroys the event.
  ~Event ();

  /// Gets the container object of event.
  Object *getObject ();

  /// Gets the type of event.
  Event::Type getType ();

  /// Gets the id of event.
  string getId ();

  /// Gets the qualified id of event.
  ///
  /// The _qualified id_ of an event is a string that uniquely identifies it
  /// in the Document.  Let `obj` be the id of the event's container object,
  /// and let `evt` be the id of the event.  Then:
  ///
  /// 1. If the event is a presentation event, its qualified id is the
  ///    string `obj@evt`.
  ///
  /// 2. If the event is an attribution event, its qualified id is the
  ///    string `obj.evt`.
  ///
  /// 3. If the event is an attribution event, its qualified id is the
  ///    string `obj<evt>`.
  ///
  string getQualifiedId ();

  /// Gets the current state of event.
  Event::State getState ();

  /// Sets the current state of event (does not trigger links).
  void setState (Event::State state);

  /// Gets the begin time of event.
  Time getBeginTime ();

  /// Sets the begin time of event.
  void setBeginTime (Time time);

  /// Gets the end time of event.
  Time getEndTime ();

  /// Sets the end time of event.
  void setEndTime (Time time);

  /// Gets the label associated with this event ("" means none).
  string getLabel ();

  /// Sets the label associated with this event.
  void setLabel (const string &label);

  // TODO ------------------------------------------------------------------

  bool isLambda ();
  bool getParameter (const string &, string *);
  bool setParameter (const string &, const string &);
  bool transition (Event::Transition);

private:

  /// The Lua state associated with the container object.
  lua_State *_L;

  /// Extra parameters to be used in event transitions.
  map<string, string> _parameters;
};

/// Action.
typedef struct
{
  Event *event;                 ///< Target event.
  Event::Transition transition; ///< Desired transition.
  Predicate *predicate;         ///< Predicate conditioning the execution.
  string value;                 ///< Value to set (if attribution).
  string duration;              ///< Duration.
  string delay;                 ///< Delay.
} Action;

GINGA_NAMESPACE_END

#endif // GINGA_EVENT_H
