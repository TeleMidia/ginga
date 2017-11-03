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
#include "NclSwitch.h"

GINGA_NAMESPACE_BEGIN

NclSwitch::NclSwitch (NclDocument *ncl, const string &id)
  :NclComposition (ncl, id)
{
}

NclSwitch::~NclSwitch ()
{
  for (auto item: _rules)
    delete item.first;
}

void
NclSwitch::addNode (NclNode *node, Predicate *pred)
{
  g_assert_nonnull (node);
  g_assert_nonnull (pred);
  NclComposition::addNode (node);
  _rules.push_back (std::make_pair (node, pred));
}

NclNode *
NclSwitch::getNode (const string &id)
{
  for (auto item: _rules)
    if (item.first->getId () == id)
      return item.first;
  return nullptr;

}

const vector <pair<NclNode *, Predicate *>> *
NclSwitch::getRules ()
{
  return &_rules;
}

GINGA_NAMESPACE_END
