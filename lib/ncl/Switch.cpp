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
#include "Switch.h"
#include "SwitchPort.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new switch.
 * @param ncl Parent document.
 * @param id Switch id.
 */
Switch::Switch (NclDocument *ncl, const string &id) : Composition (ncl, id)
{
  _default = nullptr;
}

/**
 * @brief Destroys switch.
 */
Switch::~Switch ()
{
  for (auto item: _rules)
    delete item.first;
}

/**
 * @brief Adds child node and associated predicate.
 * @param node Child node.
 * @param pred Associated predicate.
 */
void
Switch::addNode (Node *node, Predicate *pred)
{
  g_assert_nonnull (node);
  g_assert_nonnull (pred);
  Composition::addNode (node);
  _rules.push_back (std::make_pair (node, pred));
}

/**
 * @brief Gets child node.
 * @param id Node id.
 * @return Child node if successful, or null if not found.
 */
Node *
Switch::getNode (const string &id)
{
  for (auto item: _rules)
    if (item.first->getId () == id)
      return item.first;
  return nullptr;

}

/**
 * @brief Gets default node.
 */
Node *
Switch::getDefault ()
{
  return _default;
}

/**
 * @brief Sets default node.
 */
void
Switch::initDefault (Node *node)
{
  g_assert_null (_default);
  g_assert_nonnull (node);
  _default = node;
}

/**
 * @brief Gets all rules.
 */
const vector <pair<Node *, Predicate *>> *
Switch::getRules ()
{
  return &_rules;
}

/**
 * @brief Gets interface mapped by port.
 */
Anchor *
Switch::getMapInterface (Port *port)
{
  if (instanceof (SwitchPort *, port))
    return port;
  else
    return Composition::getMapInterface (port);
}

GINGA_NCL_END
