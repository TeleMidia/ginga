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
     /// Attribution of a value to a property of the object.
     ATTRIBUTION  = 1 << 1,

     /// Presentation of a time interval of the object.
     PRESENTATION = 1 << 2,

     /// Key press/release directed to the object.
     SELECTION    = 1 << 3,
  };

  /// Converts Event::Type to a human-readable string.
  static string getTypeAsString (Event::Type type);

  /// Possible states for NCL events.
  enum State
  {
     OCCURRING,                 ///< The event is occurring.
     PAUSED,                    ///< The event is paused.
     SLEEPING,                  ///< The event is sleeping.
  };

  /// Converts Event::State to a human-readable string.
  static string getStateAsString (Event::State state);

  /// Possible transitions between states of an NCL event.
  enum Transition
  {
     ABORT,                     ///< `Occurring|Paused->Sleeping`.
     PAUSE,                     ///< `Occurring->Paused`.
     RESUME,                    ///< `Paused->Occurring`.
     START,                     ///< `Paused|Sleeping->Occurring`.
     STOP,                      ///< `Occurring|Paused->Sleeping`.
  };

  /// Converts Event::Transition to a human-readable string.
  static string getTransitionAsString (Event::Transition trans);

  /// Creates a new event.
  ///
  /// @param type The type of the new event.
  /// @param object The container object.
  /// @param id The id of the new event (must no occur in \p object).
  /// @return The newly created event.
  ///
  /// @warning Do not call this constructor directly.  Use
  /// Document::createEvent() or Object::createEvent() to create events.
  ///
  Event (Event::Type type, Object *object, const string &id);

  /// Destroys the event.
  ~Event ();

  /// Gets a string representation of event.
  string toString ();

  /// Gets the type of event.
  Event::Type getType ();

  /// Gets the container object of event.
  Object *getObject ();

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

  // TODO ------------------------------------------------------------------

  bool isLambda ();
  void getInterval (Time *, Time *);
  void setInterval (Time, Time);

  bool hasLabel ();
  std::string getLabel ();
  void setLabel (const std::string &);

  bool getParameter (const string &, string *);
  bool setParameter (const string &, const string &);

  bool transition (Event::Transition);
  void reset ();

  static Event::Transition getStringAsTransition (string str);

private:

  /// The type of this event.
  Event::Type _type;

  /// The container object of this event.
  Object *_object;

  /// The Lua state associated with the container object.
  lua_State *_L;

  /// The id of this event.
  string _id;

  /// The current state of this event.
  Event::State _state;

  /// The start time of this event.
  Time _begin;

  /// The end time of this event.
  Time _end;

  /// The label associated with this event (if any).
  std::string _label;

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
