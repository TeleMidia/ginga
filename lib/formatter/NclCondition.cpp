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

#include "NclCondition.h"
#include "NclLinkStatement.h"
#include "aux-ginga.h"

GINGA_FORMATTER_BEGIN

NclCondition::NclCondition
(NclEvent *event, EventStateTransition transition)
{
  _transition = transition;
  _event = event;
  _event->addListener (this);
  _listener = nullptr;
}

NclCondition::~NclCondition ()
{
}

NclEvent *
NclCondition::getEvent ()
{
  return _event;
}

void
NclCondition::setTriggerListener (INclConditionListener *listener)
{
  g_assert_nonnull (listener);
  _listener = listener;
}

void
NclCondition::conditionSatisfied ()
{
  _listener->conditionSatisfied ();
}

void
NclCondition::eventStateChanged (unused (NclEvent *event),
                                 EventStateTransition transition,
                                 unused (EventState prev))
{
  if (_transition == transition)
    NclCondition::conditionSatisfied ();
}

GINGA_FORMATTER_END
