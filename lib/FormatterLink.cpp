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

FormatterLink::FormatterLink ()
{
  _disabled = false;
}

FormatterLink::~FormatterLink ()
{
  for (auto condition: _conditions)
    delete condition;
  for (auto action: _actions)
    delete action;
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
  tryinsert (condition, _conditions, push_back);
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

bool
FormatterLink::getDisabled ()
{
  return _disabled;
}

void
FormatterLink::setDisabled (bool disabled)
{
  _disabled = disabled;
}

GINGA_NAMESPACE_END
