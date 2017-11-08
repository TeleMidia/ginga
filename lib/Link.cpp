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
#include "Link.h"

GINGA_NAMESPACE_BEGIN

Link::Link ()
{
  _disabled = false;
}

Link::~Link ()
{
  for (auto condition: _conditions)
    delete condition;
  for (auto action: _actions)
    delete action;
}

const list<Condition *> *
Link::getConditions ()
{
  return &_conditions;
}

void
Link::addCondition (Condition *condition)
{
  g_assert_nonnull (condition);
  tryinsert (condition, _conditions, push_back);
}

const list<Action *> *
Link::getActions ()
{
  return &_actions;
}

void
Link::addAction (Action *action)
{
  g_assert_nonnull (action);
  tryinsert (action, _actions, push_back);
}

bool
Link::getDisabled ()
{
  return _disabled;
}

void
Link::setDisabled (bool disabled)
{
  _disabled = disabled;
}

GINGA_NAMESPACE_END
