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
#include "Port.h"
#include "SwitchPort.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new port.
 * @param ncl Parent document.
 * @param id Port id.
 */
Port::Port (NclDocument *ncl, const string &id) : Anchor (ncl, id)
{
  _parent = nullptr;
  _node = nullptr;
  _interface = nullptr;
}

/**
 * @brief Destroys port.
 */
Port::~Port ()
{
}

/**
 * @brief Gets port parent.
 * @return Parent composition.
 */
Composition *
Port::getParent ()
{
  return _parent;
}

/**
 * @brief initializes port parent.
 * @param parent Parent composition.
 */
void
Port::initParent (Composition *parent)
{
  g_assert_null (_parent);
  g_assert_nonnull (parent);
  _parent = parent;
}

/**
 * @brief Gets port component.
 */
Node *
Port::getNode ()
{
  return _node;
}

/**
 * @brief Sets port component.
 */
void
Port::setNode (Node *comp)
{
  g_assert_null (_node);
  g_assert_nonnull (comp);
  _node = comp;
}

/**
 * @brief Gets port interface.
 */
Anchor *
Port::getInterface ()
{
  return _interface;
}

/**
 * @brief Sets port interface.
 */
void
Port::setInterface (Anchor *iface)
{
  g_assert_null (_interface);
  g_assert_nonnull (iface);
  _interface = iface;
}

/**
 * @brief Gets final component.
 */
Node *
Port::getFinalNode ()
{
  if (instanceof (Port *, _interface))
    return cast (Port *, _interface)->getFinalNode ();
  else
    return _node;
}

/**
 * @brief Gets final interface.
 */
Anchor *
Port::getFinalInterface ()
{
  if (instanceof (Port *, _interface))
    return cast (Port *, _interface)->getFinalInterface ();
  else
    return _interface;
}

/**
 * @brief Gets list of nodes chained via port.
 */
vector<Node *>
Port::getMapNodeNesting ()
{
  vector<Node *> result;

  result.push_back (_node);
  if (!instanceof (Port *, _interface))
    return result;

  vector<Node *> aux = cast (Port *, _interface)->getMapNodeNesting ();
  for (auto node: aux)
    result.push_back (node);
  return result;
}

GINGA_NCL_END
