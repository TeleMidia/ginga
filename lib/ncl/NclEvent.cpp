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
#include "NclEvent.h"

GINGA_BEGIN

string
NclEvent::getEventTypeAsString (NclEventType type)
{
  switch (type)
    {
    case NclEventType::PRESENTATION:
      return "presentation";
    case NclEventType::ATTRIBUTION:
      return "attribution";
    case NclEventType::SELECTION:
      return "selection";
    default:
      g_assert_not_reached ();
    }
}

string
NclEvent::getEventStateAsString (NclEventState state)
{
  switch (state)
    {
    case NclEventState::SLEEPING:
      return "sleeping";
    case NclEventState::OCCURRING:
      return "occurring";
    case NclEventState::PAUSED:
      return "paused";
    default:
      g_assert_not_reached ();
    }
}

string
NclEvent::getEventStateTransitionAsString (NclEventStateTransition trans)
{
  switch (trans)
    {
    case NclEventStateTransition::START:
      return "start";
    case NclEventStateTransition::PAUSE:
      return "pause";
    case NclEventStateTransition::RESUME:
      return "resume";
    case NclEventStateTransition::STOP:
      return "stop";
    case NclEventStateTransition::ABORT:
      return "abort";
    default:
      g_assert_not_reached ();
    }
}

GINGA_END
