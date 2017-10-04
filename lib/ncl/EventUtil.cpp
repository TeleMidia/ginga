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

#include "aux-ginga.h"
#include "EventUtil.h"

GINGA_NCL_BEGIN

string
EventUtil::getEventTypeAsString (EventType type)
{
  switch (type)
    {
    case EventType::PRESENTATION:
      return "presentation";
    case EventType::ATTRIBUTION:
      return "attribution";
    case EventType::SELECTION:
      return "selection";
    default:
      g_assert_not_reached ();
    }
}

string
EventUtil::getEventStateAsString (EventState state)
{
  switch (state)
    {
    case EventState::UNKNOWN:
      return "unknown";
    case EventState::SLEEPING:
      return "sleeping";
    case EventState::OCCURRING:
      return "occurring";
    case EventState::PAUSED:
      return "paused";
    default:
      g_assert_not_reached ();
    }
}

string
EventUtil::getEventStateTransitionAsString (EventStateTransition trans)
{
  switch (trans)
    {
    case EventStateTransition::STARTS:
      return "starts";
    case EventStateTransition::PAUSES:
      return "pauses";
    case EventStateTransition::RESUMES:
      return "resumes";
    case EventStateTransition::STOPS:
      return "stops";
    case EventStateTransition::ABORTS:
      return "aborts";
    default:
      g_assert_not_reached ();
    }
}

bool
EventUtil::getTransition (EventState prev, EventState next,
                          EventStateTransition *trans)
{
  if (prev == EventState::SLEEPING)
    {
      if (next == EventState::OCCURRING)
        goto trans_start;
    }
  if (prev == EventState::OCCURRING)
    {
      if (next == EventState::SLEEPING)
        goto trans_stop;
      if (next == EventState::PAUSED)
        goto trans_pause;
    }
  if (prev == EventState::PAUSED)
    {
      if (next == EventState::OCCURRING)
        goto trans_resume;
      if (next == EventState::SLEEPING)
        goto trans_stop;
    }
  return false;

 trans_start:
  tryset (trans, EventStateTransition::STARTS);
  return true;

 trans_pause:
  tryset (trans, EventStateTransition::PAUSES);
  return true;

 trans_resume:
  tryset (trans, EventStateTransition::RESUMES);
  return true;

 trans_stop:
  tryset (trans, EventStateTransition::STOPS);
  return true;
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
