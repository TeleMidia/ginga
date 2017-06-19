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

#include "ginga.h"

GINGA_NCL_BEGIN

 // Event types.
enum class EventType {
  UNKNOWN = -1,
  SELECTION = 0,
  PRESENTATION,
  ATTRIBUTION,
  COMPOSITION
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

class EventUtil
{
public:
  // Event states.
  enum EventState {
    ST_UNKNOWN = -1,
    ST_SLEEPING = 0,
    ST_OCCURRING,
    ST_PAUSED
  };

  // Attribute types.
  enum AttributeType {
    ATT_UNKNOWN = -1,
    ATT_OCCURRENCES = 0,
    ATT_REPETITIONS,
    ATT_STATE,
    ATT_NODE_PROPERTY
  };

  static EventType
    getTypeCode (const string &typeName);

  static string
    getTypeName (EventType type);

  static EventUtil::EventState
    getStateCode (const string &stateName);

  static string
    getStateName (EventUtil::EventState state);

  static EventStateTransition
    getTransitionCode (const string &transition);

  static string
    getTransitionName (EventStateTransition transition);

  static EventUtil::AttributeType
    getAttributeTypeCode (const string &attTypeName);

  static string
    getAttributeTypeName (EventUtil::AttributeType type);
};

GINGA_NCL_END

#endif /* EVENT_UTIL_H */
