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
  this->_time = time;
  this->_event = evt;
}

int
NclEventTransition::compareTo (NclEventTransition *obj)
{
  // First compare time.
  if (_time < obj->_time)
    {
      return -1;
    }
  else if (_time > obj->_time)
    {
      return 1;
    }

  // Then, compare type.
  auto beginTrans = dynamic_cast<NclBeginEventTransition *> (this);
  auto otherBeginTrans = dynamic_cast<NclBeginEventTransition *> (obj);
  auto otherEndTrans = dynamic_cast<NclEndEventTransition *> (obj);

  if (beginTrans)
    {
      if (otherEndTrans)
        {
          return -1;
        }
      else if (_event == obj->_event)
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
      else if (_event == obj->_event)
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
  _endTrans = nullptr;
}

NclEndEventTransition::NclEndEventTransition (GingaTime time,
                                              NclPresentationEvent *event,
                                              NclBeginEventTransition *trans)
    : NclEventTransition (time, event)
{
  _beginTrans = trans;
  _beginTrans->setEndTransition (this);
}

GINGA_FORMATTER_END
