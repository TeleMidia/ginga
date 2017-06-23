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
#include "NclLinkTriggerCondition.h"

#include "mb/Display.h"
using namespace ginga::mb;

GINGA_FORMATTER_BEGIN

NclLinkTriggerCondition::NclLinkTriggerCondition () : NclLinkCondition ()
{
  _delay = 0;
  _listener = NULL;
  typeSet.insert ("NclLinkTriggerCondition");
}

NclLinkTriggerCondition::~NclLinkTriggerCondition ()
{
  _listener = NULL;
}

void
NclLinkTriggerCondition::setTriggerListener (
    NclLinkTriggerListener *listener)
{
  _listener = listener;
}

NclLinkTriggerListener *
NclLinkTriggerCondition::getTriggerListener ()
{
  return _listener;
}

GingaTime
NclLinkTriggerCondition::getDelay ()
{
  return _delay;
}

void
NclLinkTriggerCondition::setDelay (GingaTime delay)
{
  _delay = delay;
}

void
NclLinkTriggerCondition::conditionSatisfied (arg_unused (NclLinkCondition *condition))
{
  if (_delay > 0)
    ERROR_NOT_IMPLEMENTED ("condition delays are not supported");
  notifyConditionObservers (NclLinkTriggerListener::CONDITION_SATISFIED);
}

void
NclLinkTriggerCondition::notifyConditionObservers (short status)
{
  switch (status)
    {
    case NclLinkTriggerListener::CONDITION_SATISFIED:
      _listener->conditionSatisfied (this);
      break;

    case NclLinkTriggerListener::EVALUATION_STARTED:
      _listener->evaluationStarted ();
      break;

    case NclLinkTriggerListener::EVALUATION_ENDED:
      _listener->evaluationEnded ();
      break;

    default:
      g_assert_not_reached ();
    }
}

GINGA_FORMATTER_END
