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
#include "NclFormatterCausalLink.h"

#include "ExecutionObjectContext.h"

GINGA_FORMATTER_BEGIN

NclFormatterCausalLink::NclFormatterCausalLink (
    NclLinkTriggerCondition *condition, NclLinkAction *action,
    Link *ncmLink, ExecutionObjectContext *parentObject)
    : NclFormatterLink (ncmLink, parentObject)
{
  typeSet.insert ("NclFormatterCausalLink");
  this->condition = condition;
  this->action = action;

  if (this->condition != NULL)
    {
      this->condition->setTriggerListener (this);
    }

  if (this->action != NULL)
    {
      this->action->addActionProgressionListener (this);
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

NclLinkAction *
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

vector<NclEvent *> *
NclFormatterCausalLink::getEvents ()
{
  vector<NclEvent *> *events;
  vector<NclEvent *> *actEvents;
  vector<NclEvent *>::iterator i;

  events = condition->getEvents ();
  actEvents = action->getEvents ();

  if (actEvents == NULL)
    {
      return events;
    }

  if (events == NULL)
    {
      return actEvents;
    }

  for (i = actEvents->begin (); i != actEvents->end (); ++i)
    {
      events->push_back (*i);
    }

  delete actEvents;
  actEvents = NULL;

  if (events->empty ())
    {
      delete events;
      return NULL;
    }

  return events;
}

void
NclFormatterCausalLink::evaluationStarted ()
{
  /*clog << endl;
  clog << "NclFormatterCausalLink::evaluationStarted(" << ncmLink->getId();
  clog << ")" << endl << endl;*/
  parentObject->linkEvaluationStarted (this);
}

void
NclFormatterCausalLink::evaluationEnded ()
{
  /*clog << endl;
  clog << "NclFormatterCausalLink::evaluationEnded(" << ncmLink->getId();
  clog << ")" << endl << endl;*/
  parentObject->linkEvaluationFinished (this, false);
}

void
NclFormatterCausalLink::actionProcessed (bool start)
{
  /*clog << endl;
  clog << "NclFormatterCausalLink::actionProcessed(";
  if (!start) {
          clog << "start:" << ncmLink->getId();
  } else {
          clog << "nostart:" << ncmLink->getId();
  }
  clog << ")" << endl << endl;*/
  parentObject->linkEvaluationFinished (this, start);
}

GINGA_FORMATTER_END
