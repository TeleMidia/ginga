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
#include "NclConnector.h"

#include "NclAction.h"
#include "NclCondition.h"

GINGA_NAMESPACE_BEGIN


// Public.

NclConnector::NclConnector (NclDocument *ncl, const string &id)
  :NclEntity (ncl, id)
{
}

NclConnector::~NclConnector ()
{
}

const list<NclCondition *> *
NclConnector::getConditions ()
{
  return &_conditions;
}

void
NclConnector::addCondition (NclCondition *condition)
{
  g_assert_nonnull (condition);
  tryinsert (condition, _conditions, push_back);
}

const list<NclAction *> *
NclConnector::getActions ()
{
  return &_actions;
}

void
NclConnector::addAction (NclAction *action)
{
  g_assert_nonnull (action);
  tryinsert (action, _actions, push_back);
}

NclRole *
NclConnector::getRole (const string &label)
{
  for (auto conn: _conditions)
    if (conn->getLabel () == label)
      return conn;
  for (auto act: _actions)
    if (act->getLabel () == label)
      return act;
  return nullptr;
}

GINGA_NAMESPACE_END
