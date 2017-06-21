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

EventTransition::EventTransition (GingaTime time,
                                  NclPresentationEvent *evt)
{
  this->_time = time;
  this->_evt = evt;
}

int
EventTransition::compareTo (EventTransition *obj)
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
  auto beginTrans = dynamic_cast<BeginEventTransition *> (this);
  auto otherBeginTrans = dynamic_cast<BeginEventTransition *> (obj);
  auto otherEndTrans = dynamic_cast<EndEventTransition *> (obj);

  if (beginTrans)
    {
      if (otherEndTrans)
        {
          return -1;
        }
      else if (_evt == obj->_evt)
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
      else if (_evt == obj->_evt)
        {
          return 0;
        }
      else
        {
          return 1;
        }
    }
}

BeginEventTransition::BeginEventTransition (
    GingaTime t, NclPresentationEvent *evt)
  : EventTransition (t, evt)
{

}

EndEventTransition::EndEventTransition (GingaTime t,
                                        NclPresentationEvent *evt,
                                        BeginEventTransition *trans)
  : EventTransition (t, evt)
{
  _beginTrans = trans;
  _beginTrans->setEndTransition (this);
}

GINGA_FORMATTER_END
