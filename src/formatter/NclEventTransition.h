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

class NclEventTransition
{
public:
  NclEventTransition (GingaTime time, NclPresentationEvent *evt);
  virtual ~NclEventTransition () {}

  int compareTo (NclEventTransition *trans);

  NclPresentationEvent *getEvent () { return _event; }
  GingaTime getTime () { return _time; }

protected:
  set<string> typeSet;

private:
  NclPresentationEvent *_event;
  GingaTime _time;

  int compareType (NclEventTransition *other);
};

class NclEndEventTransition;

class NclBeginEventTransition : public NclEventTransition
{
  PROPERTY (NclEndEventTransition *, _endTrans, getEndTransition, setEndTransition)

public:
  NclBeginEventTransition (GingaTime time, NclPresentationEvent *evt);
  virtual ~NclBeginEventTransition () {}
};

class NclEndEventTransition : public NclEventTransition
{
  PROPERTY_READONLY (NclBeginEventTransition *, _beginTrans, getBeginTransition)

public:
  NclEndEventTransition (GingaTime time, NclPresentationEvent *evt,
                         NclBeginEventTransition *trans);

  virtual ~NclEndEventTransition () {}
};

GINGA_FORMATTER_END

#endif /*EVENTTRANSITION_H_*/
