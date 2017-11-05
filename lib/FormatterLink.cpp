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

GINGA_NAMESPACE_BEGIN

FormatterLink::FormatterLink (Formatter *formatter)
{
  g_assert_nonnull (formatter);
  _formatter = formatter;
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

const list<FormatterCondition *> *
FormatterLink::getConditions ()
{
  return &_conditions;
}

void
FormatterLink::addCondition (FormatterCondition *condition)
{
  g_assert_nonnull (condition);
  if (tryinsert (condition, _conditions, push_back))
    condition->setTriggerListener (this);
}

const list<FormatterAction *> *
FormatterLink::getActions ()
{
  return &_actions;
}

void
FormatterLink::addAction (FormatterAction *action)
{
  g_assert_nonnull (action);
  tryinsert (action, _actions, push_back);
}

void
FormatterLink::conditionSatisfied (FormatterCondition *cond)
{
  FormatterPredicate *pred;

  if (_disabled)
    return;                     // fail

  pred = cond->getPredicate ();
  if (pred != nullptr && !_formatter->evalPredicate (pred))
    return;                     // fail

  for (auto action: _actions)
    {
      FormatterEvent *evt;
      evt = action->getEvent ();
      g_assert_nonnull (evt);
      if (evt->getType () == NclEventType::ATTRIBUTION)
        {
          string dur;
          string value = "";

          if (action->getParameter ("duration", &dur))
            evt->setParameter ("duration", dur);

          action->getParameter ("value", &value);
          evt->setParameter ("value", value);
        }
      evt->transition (action->getEventStateTransition ());
    }
}

GINGA_NAMESPACE_END
