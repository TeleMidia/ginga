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
#include "NclLinkCompoundTriggerCondition.h"

GINGA_FORMATTER_BEGIN

NclLinkCompoundTriggerCondition::NclLinkCompoundTriggerCondition ()
    : NclLinkTriggerCondition ()
{
  typeSet.insert ("NclLinkCompoundTriggerCondition");
}

NclLinkCompoundTriggerCondition::~NclLinkCompoundTriggerCondition ()
{
  vector<NclLinkCondition *>::iterator i;
  NclLinkCondition *condition;

  for (i = conditions.begin (); i != conditions.end (); ++i)
    {
      condition = (NclLinkCondition *)(*i);
      if (condition != NULL)
        {
          delete condition;
          condition = NULL;
        }
    }

  conditions.clear ();
}

void
NclLinkCompoundTriggerCondition::addCondition (NclLinkCondition *condition)
{
  if (condition == NULL)
    {
      return;
    }

  conditions.push_back (condition);
  if (instanceof (NclLinkTriggerCondition *, condition))
    {
      ((NclLinkTriggerCondition *)condition)->setTriggerListener (this);
    }
}

vector<NclEvent *>
NclLinkCompoundTriggerCondition::getEvents ()
{
  vector<NclEvent *> events;

  for (NclLinkCondition *condition : conditions)
    {
      for (NclEvent *evt : condition->getEvents ())
        {
          events.push_back (evt);
        }
    }

  return events;
}

void
NclLinkCompoundTriggerCondition::conditionSatisfied (NclLinkCondition *condition)
{
  NclLinkTriggerCondition::conditionSatisfied (condition);
}

void
NclLinkCompoundTriggerCondition::evaluationStarted ()
{
  notifyConditionObservers (NclLinkTriggerListener::EVALUATION_STARTED);
}

void
NclLinkCompoundTriggerCondition::evaluationEnded ()
{
  notifyConditionObservers (NclLinkTriggerListener::EVALUATION_ENDED);
}

GINGA_FORMATTER_END
