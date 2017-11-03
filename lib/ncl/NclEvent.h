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

#ifndef NCL_EVENT_H
#define NCL_EVENT_H

#include "aux-ginga.h"

GINGA_NAMESPACE_BEGIN

enum class NclEventType
{
  SELECTION = 0,
  PRESENTATION,
  ATTRIBUTION,
};

enum class NclEventState
{
  SLEEPING = 0,
  OCCURRING,
  PAUSED
};

enum class NclEventStateTransition
{
  START = 0,
  PAUSE,
  RESUME,
  STOP,
  ABORT
};

class NclEvent
{
public:
  static string getEventTypeAsString (NclEventType);
  static string getEventStateAsString (NclEventState);
  static string getEventStateTransitionAsString (NclEventStateTransition);
};

GINGA_NAMESPACE_END

#endif // NCL_EVENT_H
