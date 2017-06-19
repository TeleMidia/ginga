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

#include "ginga.h"
#include "EventUtil.h"

GINGA_NCL_BEGIN

EventType
EventUtil::getTypeCode (const string &typeName)
{
  if (typeName == "presentation")
    {
      return EventType::PRESENTATION;
    }
  else if (typeName == "selection")
    {
      return EventType::SELECTION;
    }
  else if (typeName == "attribution")
    {
      return EventType::ATTRIBUTION;
    }
  else if (typeName == "composition")
    {
      return EventType::COMPOSITION;
    }

  return EventType::PRESENTATION;
}

string
EventUtil::getTypeName (EventType type)
{
  switch (type)
    {
    case EventType::PRESENTATION:
      return "presentation";

    case EventType::SELECTION:
      return "selection";

    case EventType::ATTRIBUTION:
      return "attribution";

    case EventType::COMPOSITION:
      return "composition";

    default:
      return "";
    }
}

EventState
EventUtil::getStateCode (const string &stateName)
{
  if (stateName == "occurring")
    {
      return EventState::OCCURRING;
    }
  else if (stateName == "paused")
    {
      return EventState::PAUSED;
    }

  // "sleeping"
  return EventState::SLEEPING;
}

string
EventUtil::getStateName (EventState state)
{
  switch (state)
    {
    case EventState::OCCURRING:
      return "occurring";

    case EventState::PAUSED:
      return "paused";

    case EventState::SLEEPING:
      return "sleeping";

    default:
      return "";
    }
}

string
EventUtil::getTransitionName (EventStateTransition transition)
{
  switch (transition)
    {
    case EventStateTransition::STARTS:
      return "starts";

    case EventStateTransition::STOPS:
      return "stops";

    case EventStateTransition::PAUSES:
      return "pauses";

    case EventStateTransition::RESUMES:
      return "resumes";

    case EventStateTransition::ABORTS:
      return "aborts";

    default:
      return "";
    }
}

EventStateTransition
EventUtil::getTransitionCode (const string &transition)
{
  if (transition == "starts")
    {
      return EventStateTransition::STARTS;
    }
  else if (transition == "stops")
    {
      return EventStateTransition::STOPS;
    }
  else if (transition == "pauses")
    {
      return EventStateTransition::PAUSES;
    }
  else if (transition == "resumes")
    {
      return EventStateTransition::RESUMES;
    }
  else if (transition == "aborts")
    {
      return EventStateTransition::ABORTS;
    }

  return EventStateTransition::UNKNOWN;
}

AttributeType
EventUtil::getAttributeTypeCode (const string &attTypeName)
{
  if (attTypeName == "occurrences")
    {
      return AttributeType::OCCURRENCES;
    }
  else if (attTypeName == "nodeProperty")
    {
      return AttributeType::NODE_PROPERTY;
    }
  else if (attTypeName == "repetitions")
    {
      return AttributeType::REPETITIONS;
    }
  else if (attTypeName == "state")
    {
      return AttributeType::STATE;
    }

  return AttributeType::UNKNOWN;
}

string
EventUtil::getAttributeTypeName (AttributeType type)
{
  switch (type)
    {
    case AttributeType::OCCURRENCES:
      return "occurrences";

    case AttributeType::NODE_PROPERTY:
      return "nodeProperty";

    case AttributeType::REPETITIONS:
      return "repetitions";

    case AttributeType::STATE:
      return "state";

    default:
      return "nodeProperty";
    }
}

GINGA_NCL_END
