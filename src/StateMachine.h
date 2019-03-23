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

#ifndef GINGA_STATE_MACHINE_H
#define GINGA_STATE_MACHINE_H

#include "aux-ginga.h"
#include "Predicate.h"

GINGA_NAMESPACE_BEGIN

class Object;

/// State machine in an NCL object.
class StateMachine
{
public:

  /// Types of state machines.
  enum Type
  {
     ATTRIBUTION,               ///< Attribution machine.
     PRESENTATION,              ///< Presentation machine
     SELECTION,                 ///< Selection machine.
  };

  /// Machine states.
  enum State
  {
     OCCURRING,                 ///< Occurring state.
     PAUSED,                    ///< Paused state.
     SLEEPING,                  ///< Sleeping state.
  };

  /// Transitions between states.
  enum Transition
  {
     ABORT,                     ///< `Occurring|Paused->Sleeping`.
     PAUSE,                     ///< `Occurring->Paused`.
     RESUME,                    ///< `Paused->Occurring`.
     START,                     ///< `Paused|Sleeping->Occurring`.
     STOP,                      ///< `Occurring|Paused->Sleeping`.
  };

  /// Creates a new state machine.
  StateMachine (Object *object, StateMachine::Type type, const string &id);

  /// Destroys the state machine.
  ~StateMachine ();

  /// Gets the container object of the state machine.
  Object *getObject ();

  /// Gets the type of state machine.
  StateMachine::Type getType ();

  /// Gets the id of state machine.
  string getId ();

  /// Gets the qualified id of state machine.
  ///
  /// The _qualified id_ of a state machine is a string that uniquely
  /// identifies it in the Document.  Let `obj` be the id of the state
  /// machine's container object, and let `sm` be the id of the state
  /// machine.  Then:
  ///
  /// 1. If the state machine is of type presentation, then its qualified id
  ///    is the string `obj@sm`.
  ///
  /// 2. If the state machine is of type attribution, then its qualified id
  ///    is the string `obj.sm`.
  ///
  /// 3. If the state machine is of type selection, its qualified id is the
  ///    string `obj<sm>`.
  ///
  string getQualifiedId ();

  /// Gets the current state of state machine.
  StateMachine::State getState ();

  /// Sets the current state of state machine.
  void setState (StateMachine::State state);

  /// Gets the begin time of (presentation) state machine.
  Time getBeginTime ();

  /// Sets the begin time of (presentation) state machine.
  void setBeginTime (Time time);

  /// Gets the end time of (presentation) state machine.
  Time getEndTime ();

  /// Sets the end time of (presentation) state machine.
  void setEndTime (Time time);

  /// Gets the label associated with this state machine ("" means none).
  string getLabel ();

  /// Sets the label associated with this state machine.
  void setLabel (const string &label);

  /// Transitions the state machine.
  bool transition (StateMachine::Transition transition,
                   const map<string, string> &params);

private:

  /// The Lua state of the container object.
  lua_State *_L;
};

/// Action.
typedef struct
{
  string target;                       ///< Target state machine.
  StateMachine::Transition transition; ///< Desired transition.
  Predicate *predicate;                ///< Predicate.
  map<string, string> params;          ///< Parameters.
} Action;

GINGA_NAMESPACE_END

#endif // GINGA_STATE_MACHINE_H
