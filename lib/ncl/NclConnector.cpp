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

GINGA_BEGIN


// Public.

NclConnector::NclConnector (NclDocument *ncl, const string &id)
  :NclEntity (ncl, id)
{
}

NclConnector::~NclConnector ()
{
  // for (auto item: _predicates)
  //   delete item.second;
}

const vector <NclCondition *> *
NclConnector::getConditions ()
{
  return &_conditions;
}

bool
NclConnector::addCondition (NclCondition *condition)
{
  g_assert_nonnull (condition);
  for (auto cond: _conditions)
    if (cond == condition)
      return false;
  _conditions.push_back (condition);
  return true;
}

const vector<NclAction *> *
NclConnector::getActions ()
{
  return &_actions;
}

bool
NclConnector::addAction (NclAction *action)
{
  g_assert_nonnull (action);
  for (auto act: _actions)
    if (act == action)
      return false;
  _actions.push_back (action);
  return true;
}

// void
// NclConnector::addPredicateRole (const string &role, Predicate *pred)
// {
//   g_assert_nonnull (pred);
//   g_assert_null (this->getRole (role));
//   _predicates[role] = pred;
// }

// Predicate *
// NclConnector::getPredicateRole (const string &role)
// {
//   if (_predicates.find (role) == _predicates.end ())
//     return nullptr;
//   return _predicates[role];
// }

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

GINGA_END
