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

NclFormatterLink::NclFormatterLink (
    NclLinkTriggerCondition *condition, NclAction *act,
    Link *ncmLink, ExecutionObjectContext *parentObj)
{
  this->parentObject = parentObj; // ExecutionObjectContext
  this->ncmLink = ncmLink;
  this->suspend = false;

  this->condition = condition;
  this->action = act;

  if (this->condition != nullptr)
    {
      this->condition->setTriggerListener (this);
    }

  if (this->action != nullptr)
    {
      this->action->addProgressListener (this);
    }
}

NclFormatterLink::~NclFormatterLink ()
{
  if (condition != nullptr)
    {
      delete condition;
    }

  if (action != nullptr)
    {
      delete action;
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

NclAction *
NclFormatterLink::getAction ()
{
  return action;
}

NclLinkTriggerCondition *
NclFormatterLink::getTriggerCondition ()
{
  return condition;
}

void
NclFormatterLink::conditionSatisfied (NclLinkCondition *condition)
{
  if (!suspend)
    {
      action->run (condition);
    }
}

vector<NclEvent *>
NclFormatterLink::getEvents ()
{
  vector<NclEvent *> events = condition->getEvents ();
  vector<NclEvent *> actEvents = action->getEvents ();

  events.insert(events.end(), actEvents.begin(), actEvents.end());

  return events;
}

void
NclFormatterLink::evaluationStarted ()
{
  parentObject->linkEvaluationStarted (this);
}

void
NclFormatterLink::evaluationEnded ()
{
  parentObject->linkEvaluationFinished (this, false);
}

void
NclFormatterLink::actionProcessed (bool start)
{
  parentObject->linkEvaluationFinished (this, start);
}

GINGA_FORMATTER_END
