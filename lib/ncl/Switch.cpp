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
  _nodes.clear ();
  _rules.clear ();
}

/**
 * @brief Adds child node and associated rule.
 * @param node Child node.
 * @param rule Associated rule.
 */
void
Switch::addNode (Node *node, Rule *rule)
{
  g_assert_nonnull (node);
  _nodes.push_back (node);
  g_assert_nonnull (rule);
  _rules.push_back (rule);
  node->initParent (this);
}

/**
 * @brief Updates default node.
 */
void
Switch::addNode (Node *node)
{
  g_assert_nonnull (node);
  this->setDefaultNode (node);
}

/**
 * @brief Gets child node.
 * @param id Node id.
 * @return Child node if successful, or null if not found.
 */
Node *
Switch::getNode (const string &id)
{
  if (_default != nullptr && _default->getId () == id)
    return _default;
  return Composition::getNode (id);
}

/**
 * @brief Gets default node.
 */
Node *
Switch::getDefaultNode ()
{
  return _default;
}

/**
 * @brief Sets default node.
 */
void
Switch::setDefaultNode (Node *node)
{
  g_assert_null (_default);
  g_assert_nonnull (node);
  _default = node;
}

/**
 * @brief Gets all rules.
 */
const vector <Rule *> *
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
