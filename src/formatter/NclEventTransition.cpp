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

NclEventTransition::NclEventTransition (GingaTime time,
                                        NclPresentationEvent *evt)
{
  typeSet.insert ("NclEventTransition");
  this->_time = time;
  this->_event = evt;
}

int
NclEventTransition::compareTo (NclEventTransition *object)
{
  NclEventTransition *otherEntry = object;

  if (_time < otherEntry->_time)
    {
      return -1;
    }
  else if (_time > otherEntry->_time)
    {
      return 1;
    }
  else
    {
      return compareType (otherEntry);
    }
}

bool
NclEventTransition::instanceOf (const string &s)
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
NclEventTransition::compareType (NclEventTransition *other)
{
  auto beginTrans = dynamic_cast<NclBeginEventTransition *> (this);

  auto otherBeginTrans = dynamic_cast<NclBeginEventTransition *> (other);
  auto otherEndTrans = dynamic_cast<NclEndEventTransition *> (other);

  if (beginTrans)
    {
      if (otherEndTrans)
        {
          return -1;
        }
      else if (_event == other->_event)
        {
          return 0;
        }
      else
        {
          return 1;
        }
    }
  else
    {
      if (otherBeginTrans)
        {
          return 1;
        }
      else if (_event == other->_event)
        {
          return 0;
        }
      else
        {
          return 1;
        }
    }
}

NclBeginEventTransition::NclBeginEventTransition (
    GingaTime time, NclPresentationEvent *event)
    : NclEventTransition (time, event)
{
  typeSet.insert ("NclBeginEventTransition");
  _endTrans = nullptr;
}

NclEndEventTransition::NclEndEventTransition (GingaTime time,
                                              NclPresentationEvent *event,
                                              NclBeginEventTransition *trans)
    : NclEventTransition (time, event)
{
  typeSet.insert ("NclEndEventTransition");
  _beginTrans = trans;
  _beginTrans->setEndTransition (this);
}

GINGA_FORMATTER_END
