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
#include "NclFormatterLink.h"

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"

GINGA_FORMATTER_BEGIN

NclFormatterLink::NclFormatterLink (Link *ncmLink,
                                    ExecutionObjectContext *parentObject)
{
  this->parentObject = parentObject; // ExecutionObjectContext
  this->ncmLink = ncmLink;
  this->suspend = false;
}

NclFormatterLink::~NclFormatterLink ()
{
  parentObject = NULL;
  if (ncmLink != NULL)
    {
      ncmLink = NULL;
    }
}

void
NclFormatterLink::suspendLinkEvaluation (bool suspend)
{
  this->suspend = suspend;
}

Link *
NclFormatterLink::getNcmLink ()
{
  return ncmLink;
}

NclFormatterCausalLink::NclFormatterCausalLink (
    NclLinkTriggerCondition *condition, NclAction *action,
    Link *ncmLink, ExecutionObjectContext *parentObject)
    : NclFormatterLink (ncmLink, parentObject)
{
  this->condition = condition;
  this->action = action;

  if (this->condition != NULL)
    {
      this->condition->setTriggerListener (this);
    }

  if (this->action != NULL)
    {
      this->action->addProgressListener (this);
    }
}

NclFormatterCausalLink::~NclFormatterCausalLink ()
{
  if (condition != NULL)
    {
      delete condition;
      condition = NULL;
    }

  if (action != NULL)
    {
      delete action;
      action = NULL;
    }
}

NclAction *
NclFormatterCausalLink::getAction ()
{
  return action;
}

NclLinkTriggerCondition *
NclFormatterCausalLink::getTriggerCondition ()
{
  return condition;
}

void
NclFormatterCausalLink::conditionSatisfied (NclLinkCondition *condition)
{
  if (!suspend)
    {
      action->run (condition);
    }
}

vector<NclEvent *>
NclFormatterCausalLink::getEvents ()
{
  vector<NclEvent *> events = condition->getEvents ();
  vector<NclEvent *> actEvents = action->getEvents ();

  events.insert(events.end(), actEvents.begin(), actEvents.end());

  return events;
}

void
NclFormatterCausalLink::evaluationStarted ()
{
  parentObject->linkEvaluationStarted (this);
}

void
NclFormatterCausalLink::evaluationEnded ()
{
  parentObject->linkEvaluationFinished (this, false);
}

void
NclFormatterCausalLink::actionProcessed (bool start)
{
  parentObject->linkEvaluationFinished (this, start);
}

GINGA_FORMATTER_END
