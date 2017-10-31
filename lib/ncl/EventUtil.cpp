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
    case EventType::PROXY:
      return "proxy";
    default:
      g_assert_not_reached ();
    }
}

string
EventUtil::getEventStateAsString (EventState state)
{
  switch (state)
    {
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
    case EventStateTransition::START:
      return "starts";
    case EventStateTransition::PAUSE:
      return "pauses";
    case EventStateTransition::RESUME:
      return "resumes";
    case EventStateTransition::STOP:
      return "stops";
    case EventStateTransition::ABORT:
      return "aborts";
    default:
      g_assert_not_reached ();
    }
}

GINGA_NCL_END
