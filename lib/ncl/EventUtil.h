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

#ifndef EVENT_UTIL_H
#define EVENT_UTIL_H

GINGA_NCL_BEGIN

// Event types.
enum class EventType {
  SELECTION = 0,
  PRESENTATION,
  ATTRIBUTION,
};

// Event states.
enum class EventState {
  UNKNOWN = -1,
  SLEEPING = 0,
  OCCURRING,
  PAUSED
};

// Event state transitions.
enum class EventStateTransition {
  UNKNOWN = -1,
  STARTS = 0,
  STOPS,
  PAUSES,
  RESUMES,
  ABORTS
};

// Attribute types.
enum class AttributeType {
  OCCURRENCES = 0,
  REPETITIONS,
  STATE,
  NODE_PROPERTY
};

class EventUtil
{
public:
  static string
    getStateName (EventState state);

  static EventStateTransition
    getTransition (EventState previousState, EventState nextState);

  static EventState
    getNextState (EventStateTransition transition);
};

GINGA_NCL_END

#endif /* EVENT_UTIL_H */
