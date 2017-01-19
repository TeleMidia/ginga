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
#include "NclEventTransition.h"

GINGA_FORMATTER_BEGIN

NclEventTransition::NclEventTransition (double time,
                                        NclPresentationEvent *event)
{
  typeSet.insert ("NclEventTransition");
  this->time = time;
  this->event = event;
}

NclEventTransition::~NclEventTransition () {}

int
NclEventTransition::compareTo (NclEventTransition *object)
{
  NclEventTransition *otherEntry;

  if (object->instanceOf ("NclEventTransition"))
    {
      otherEntry = (NclEventTransition *)object;

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
NclEventTransition::instanceOf (string s)
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
NclEventTransition::compareType (NclEventTransition *otherEntry)
{
  if (this->instanceOf ("NclBeginEventTransition"))
    {
      if (otherEntry->instanceOf ("NclEndEventTransition"))
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
      if (otherEntry->instanceOf ("NclBeginEventTransition"))
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
NclEventTransition::equals (NclEventTransition *object)
{
  switch (compareTo (object))
    {
    case 0:
      return true;

    default:
      return false;
    }
}

NclPresentationEvent *
NclEventTransition::getEvent ()
{
  return event;
}

double
NclEventTransition::getTime ()
{
  return time;
}

GINGA_FORMATTER_END
