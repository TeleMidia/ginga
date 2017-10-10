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
#include "NclLink.h"
#include "ExecutionObjectContext.h"

GINGA_FORMATTER_BEGIN

NclLink::NclLink (ExecutionObjectContext *context)
{
  _context = context;
  _disabled = false;
}

NclLink::~NclLink ()
{
  for (auto condition: _conditions)
    delete condition;
  for (auto action: _actions)
    delete action;
}

void
NclLink::disable (bool disable)
{
  _disabled = disable;
}

const vector <NclCondition *> *
NclLink::getConditions ()
{
  return &_conditions;
}

bool
NclLink::addCondition (NclCondition *condition)
{
  g_assert_nonnull (condition);
  for (auto cond: _conditions)
    if (cond == condition)
      return false;
  condition->setTriggerListener (this);
  _conditions.push_back (condition);
  return true;
}

const vector <NclAction *> *
NclLink::getActions ()
{
  return &_actions;
}

bool
NclLink::addAction (NclAction *action)
{
  g_assert_nonnull (action);
  for (auto act: _actions)
    if (act == action)
      return false;
  _actions.push_back (action);
  return true;
}

void
NclLink::conditionSatisfied ()
{
  if (_disabled)
    return;                     // nothing to do
  for (auto action: _actions)
    action->run ();
}

vector<NclEvent *>
NclLink::getEvents ()
{
  vector<NclEvent *> events;
  for (auto condition: _conditions)
    events.push_back (condition->getEvent ());
  for (auto action: _actions)
    events.push_back (action->getEvent ());
  return events;
}

void
NclLink::evaluationStarted ()
{
  _context->linkEvaluationStarted (this);
}

void
NclLink::evaluationEnded ()
{
  _context->linkEvaluationFinished (this);
}

GINGA_FORMATTER_END
