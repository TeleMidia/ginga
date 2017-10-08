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

#include "aux-ginga.h"
#include "NclFormatterLink.h"

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"

GINGA_FORMATTER_BEGIN

NclFormatterLink::NclFormatterLink (
    NclLinkTriggerCondition *condition, NclAction *act,
    Link *ncmLink, ExecutionObjectContext *parentObj)
{
  this->_parentObj = parentObj;
  this->_ncmLink = ncmLink;
  this->_suspended = false;

  this->_condition = condition;
  this->_action = act;

  g_assert_nonnull (this->_condition);
  g_assert_nonnull (this->_action);

  this->_condition->setTriggerListener (this);
  //this->_action->addProgressListener (this);
}

NclFormatterLink::~NclFormatterLink ()
{
  if (_condition != nullptr)
    delete _condition;

  if (_action != nullptr)
    delete _action;
}

void
NclFormatterLink::suspendLinkEvaluation (bool suspend)
{
  this->_suspended = suspend;
}

Link *
NclFormatterLink::getNcmLink ()
{
  return _ncmLink;
}

NclAction *
NclFormatterLink::getAction ()
{
  return _action;
}

NclLinkTriggerCondition *
NclFormatterLink::getTriggerCondition ()
{
  return _condition;
}

void
NclFormatterLink::conditionSatisfied (unused (NclLinkCondition *condition))
{
  if (!_suspended)
    _action->run ();
}

vector<NclEvent *>
NclFormatterLink::getEvents ()
{
  vector<NclEvent *> events = _condition->getEvents ();
  vector<NclEvent *> actEvents = _action->getEvents ();

  events.insert(events.end(), actEvents.begin(), actEvents.end());

  return events;
}

void
NclFormatterLink::evaluationStarted ()
{
  _parentObj->linkEvaluationStarted (this);
}

void
NclFormatterLink::evaluationEnded ()
{
  _parentObj->linkEvaluationFinished (this, false);
}

// void
// NclFormatterLink::actionProcessed (bool start)
// {
//   _parentObj->linkEvaluationFinished (this, start);
// }

GINGA_FORMATTER_END
