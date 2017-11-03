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
#include "FormatterLink.h"
#include "FormatterContext.h"
#include "FormatterScheduler.h"

GINGA_NAMESPACE_BEGIN

FormatterLink::FormatterLink (Formatter *ginga)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  _disabled = false;
}

FormatterLink::~FormatterLink ()
{
  for (auto condition: _conditions)
    delete condition;
  for (auto action: _actions)
    delete action;
}

void
FormatterLink::disable (bool disable)
{
  _disabled = disable;
}

const vector <FormatterCondition *> *
FormatterLink::getConditions ()
{
  return &_conditions;
}

bool
FormatterLink::addCondition (FormatterCondition *condition)
{
  g_assert_nonnull (condition);
  for (auto cond: _conditions)
    if (cond == condition)
      return false;
  condition->setTriggerListener (this);
  _conditions.push_back (condition);
  return true;
}

const vector <FormatterAction *> *
FormatterLink::getActions ()
{
  return &_actions;
}

bool
FormatterLink::addAction (FormatterAction *action)
{
  g_assert_nonnull (action);
  for (auto act: _actions)
    if (act == action)
      return false;
  _actions.push_back (action);
  return true;
}

void
FormatterLink::conditionSatisfied (FormatterCondition *cond)
{
  FormatterPredicate *pred;

  if (_disabled)
    return;                     // nothing to do

  pred = cond->getPredicate ();
  if (pred != nullptr && !_scheduler->eval (pred))
    return;                     // nothing to do

  for (auto action: _actions)
    {
      FormatterEvent *evt;
      evt = action->getEvent ();
      g_assert_nonnull (evt);
      if (evt->getType () == NclEventType::ATTRIBUTION)
        {
          evt->setParameter ("duration", action->getDuration ());
          evt->setParameter ("value", action->getValue ());
        }
      evt->transition (action->getEventStateTransition ());
    }
}

vector<FormatterEvent *>
FormatterLink::getEvents ()
{
  vector<FormatterEvent *> events;
  for (auto condition: _conditions)
    events.push_back (condition->getEvent ());
  for (auto action: _actions)
    events.push_back (action->getEvent ());
  return events;
}

GINGA_NAMESPACE_END
