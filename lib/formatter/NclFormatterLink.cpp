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

NclFormatterLink::NclFormatterLink (NclLinkTriggerCondition *condition,
                                    Link *ncmLink,
                                    ExecutionObjectContext *parentObj)
{
  _parentObj = parentObj;
  _ncmLink = ncmLink;
  _suspended = false;
  _condition = condition;
  g_assert_nonnull (_condition);
  _condition->setTriggerListener (this);
}

NclFormatterLink::~NclFormatterLink ()
{
  if (_condition != nullptr)
    delete _condition;

  for (auto action: _actions)
    delete action;
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

const vector <NclSimpleAction *> *
NclFormatterLink::getActions ()
{
  return &_actions;
}

void
NclFormatterLink::addAction (NclSimpleAction *action)
{
  g_assert_nonnull (action);
  _actions.push_back (action);
}

NclLinkTriggerCondition *
NclFormatterLink::getTriggerCondition ()
{
  return _condition;
}

void
NclFormatterLink::conditionSatisfied ()
{
  if (_suspended)
    return;                     // nothing to do
  for (auto action: _actions)
    action->run ();
}

vector<NclEvent *>
NclFormatterLink::getEvents ()
{
  vector<NclEvent *> events = _condition->getEvents ();
  for (auto action: _actions)
    events.push_back (action->getEvent ());
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

GINGA_FORMATTER_END
