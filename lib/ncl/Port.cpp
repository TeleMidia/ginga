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
 * @brief Gets target node.
 * @return Node.
 */
Node *
Port::getNode ()
{
  return _node;
}

/**
 * @brief Initializes target node.
 * @param node Node.
 */
void
Port::initNode (Node *node)
{
  g_assert_null (_node);
  g_assert_nonnull (node);
  _node = node;
}

/**
 * @brief Gets target interface.
 * @return Interface.
 */
Anchor *
Port::getInterface ()
{
  return _interface;
}

/**
 * @brief Initializes target interface.
 * @param iface Interface.
 */
void
Port::initInterface (Anchor *iface)
{
  g_assert_null (_interface);
  g_assert_nonnull (iface);
  _interface = iface;
}

/**
 * @brief Gets final targets.
 * @param node Address of variable to store target node.
 * @param iface Address of variable to store target interface.
 */
void
Port::getTarget (Node **node, Anchor **iface)
{
  if (node != nullptr)
    {
      Node *target = this->getFinalNode ();
      g_assert_nonnull (target);
      *node = target;
    }
  if (iface != nullptr)
    {
      Anchor *target = this->getFinalInterface ();
      g_assert_nonnull (target);
      *iface = target;
    }
}


// Private.

Node *
Port::getFinalNode ()
{
  if (instanceof (Port *, _interface))
    return cast (Port *, _interface)->getFinalNode ();
  else
    return _node;
}

Anchor *
Port::getFinalInterface ()
{
  if (instanceof (Port *, _interface))
    return cast (Port *, _interface)->getFinalInterface ();
  else
    return _interface;
}

GINGA_NCL_END
