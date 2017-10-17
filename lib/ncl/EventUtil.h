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

enum class EventType
{
  SELECTION = 0,
  PRESENTATION,
  ATTRIBUTION,
};

enum class EventState
{
  SLEEPING = 0,
  OCCURRING,
  PAUSED
};

enum class EventStateTransition
{
  START = 0,
  PAUSE,
  RESUME,
  STOP,
  ABORT
};

class EventUtil
{
public:
  static string getEventTypeAsString (EventType);
  static string getEventStateAsString (EventState);
  static string getEventStateTransitionAsString (EventStateTransition);

  static bool getTransition (EventState, EventState, EventStateTransition *);
};

GINGA_NCL_END

#endif // EVENT_UTIL_H
