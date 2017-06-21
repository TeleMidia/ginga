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

#ifndef EVENTTRANSITION_H_
#define EVENTTRANSITION_H_

#include "FormatterEvents.h"

GINGA_FORMATTER_BEGIN

class EventTransition
{
  PROPERTY_READONLY (NclPresentationEvent *, _evt, getEvent)
  PROPERTY_READONLY (GingaTime, _time, getTime)

public:
  EventTransition (GingaTime time, NclPresentationEvent *evt);
  virtual ~EventTransition () {}

  int compareTo (EventTransition *trans);
};

class EndEventTransition;

class BeginEventTransition : public EventTransition
{
  PROPERTY (EndEventTransition *, _endTrans, getEndTransition, setEndTransition)

public:
  BeginEventTransition (GingaTime time, NclPresentationEvent *evt);
};

class EndEventTransition : public EventTransition
{
  PROPERTY_READONLY (BeginEventTransition *, _beginTrans, getBeginTransition)

public:
  EndEventTransition (GingaTime t, NclPresentationEvent *evt,
                      BeginEventTransition *trans);
};

GINGA_FORMATTER_END

#endif /*EVENTTRANSITION_H_*/
