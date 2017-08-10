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

#include "ginga-internal.h"
#include "EventUtil.h"

GINGA_NCL_BEGIN

EventStateTransition
EventUtil::getTransition (EventState previous, EventState next)
{
  switch (previous)
    {
    case EventState::SLEEPING:
      return (next == EventState::OCCURRING)
        ? EventStateTransition::STARTS
        : EventStateTransition::UNKNOWN;
      break;

    case EventState::OCCURRING:
      switch (next)
        {
        case EventState::SLEEPING:
          return EventStateTransition::STOPS;
        case EventState::PAUSED:
          return EventStateTransition::PAUSES;
        default:
          return EventStateTransition::UNKNOWN;
        }
      break;

    case EventState::PAUSED:
      switch (next)
        {
        case EventState::OCCURRING:
          return EventStateTransition::RESUMES;
        case EventState::SLEEPING:
          return EventStateTransition::STOPS;
        default:
          return EventStateTransition::UNKNOWN;
        }
      break;

    default:
      g_assert_not_reached ();
    }

  g_assert_not_reached ();
}

EventState
EventUtil::getNextState (EventStateTransition transition)
{
  switch (transition)
    {
    case EventStateTransition::STOPS:
      return EventState::SLEEPING;

    case EventStateTransition::STARTS:
    case EventStateTransition::RESUMES:
      return EventState::OCCURRING;

    case EventStateTransition::PAUSES:
      return EventState::PAUSED;

    case EventStateTransition::ABORTS:
      return EventState::SLEEPING;

    default:
      return EventState::UNKNOWN;
    }
}

GINGA_NCL_END
