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

#include "config.h"
#include "EventTransition.h"

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_BEGIN

EventTransition::EventTransition (double time, PresentationEvent *event)
{
  typeSet.insert ("EventTransition");
  this->time = time;
  this->event = event;
}

EventTransition::~EventTransition () {}

int
EventTransition::compareTo (EventTransition *object)
{
  EventTransition *otherEntry;

  if (object->instanceOf ("EventTransition"))
    {
      otherEntry = (EventTransition *)object;

      if (otherEntry->time < 0 && time >= 0)
        {
          return -1;
        }
      else if (time < 0 && otherEntry->time >= 0)
        {
          return 1;
        }
      else if (time < 0 && otherEntry->time < 0)
        {
          return compareType (otherEntry);
        }
      else if (time < otherEntry->time)
        {
          return -1;
        }
      else if (time > otherEntry->time)
        {
          return 1;
        }
      else
        {
          return compareType (otherEntry);
        }
    }
  else
    {
      return -1;
    }
  return -1;
}

bool
EventTransition::instanceOf (string s)
{
  if (typeSet.empty ())
    {
      return false;
    }
  else
    {
      return (typeSet.find (s) != typeSet.end ());
    }
}

int
EventTransition::compareType (EventTransition *otherEntry)
{
  if (this->instanceOf ("BeginEventTransition"))
    {
      if (otherEntry->instanceOf ("EndEventTransition"))
        {
          return -1;
        }
      else if (event == otherEntry->event)
        {
          return 0;

          /*} else if (event.hashCode() < other_entry.event.hashCode()) {
              return -1;*/
        }
      else
        {
          return 1;
        }
    }
  else
    {
      if (otherEntry->instanceOf ("BeginEventTransition"))
        {
          return 1;
        }
      else if (event == otherEntry->event)
        {
          return 0;

          /*} else if (event.hashCode() < other_entry.event.hashCode()) {
                  return -1;*/
        }
      else
        {
          return 1;
        }
    }
}

bool
EventTransition::equals (EventTransition *object)
{
  switch (compareTo (object))
    {
    case 0:
      return true;

    default:
      return false;
    }
}

PresentationEvent *
EventTransition::getEvent ()
{
  return event;
}

double
EventTransition::getTime ()
{
  return time;
}

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_END
