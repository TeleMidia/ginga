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
#include "Composition.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new composition.
 * @param ncl Parent document.
 * @param id Composition id.
 */
Composition::Composition (NclDocument *ncl, const string &id)
  : Node (ncl, id)
{
}

/**
 * @brief Destroys composition.
 */
Composition::~Composition ()
{
  for (auto node: _nodes)
    delete node;
  for (auto port: _ports)
    delete port;
  _nodes.clear ();
  _ports.clear ();
}

/**
 * @brief Adds child node to composition.
 */
void
Composition::addNode (Node *node)
{
  g_assert_nonnull (node);
  _nodes.push_back (node);
  node->initParent (this);
}

/**
 * @brief Gets all child nodes.
 */
const vector<Node *> *
Composition::getNodes ()
{
  return &_nodes;
}

/**
 * @brief Gets child node.
 * @param id Node id.
 * @return Child node if successful, or null if not found.
 */
Node *
Composition::getNode (const string &id)
{
  for (auto node: _nodes)
    if (node->getId () == id)
      return node;
  return nullptr;
}

/**
 * @brief Gets descendant node.
 * @param id Node id.
 * @return Descendant node if successful, or null if not found.
 */
Node *
Composition::getNestedNode (const string &id)
{
  for (auto node: _nodes)
    {
      if (node->getId () == id)
        return node;
      if (instanceof (Composition *, node))
        {
          Node *out = cast (Composition *, node)->getNestedNode (id);
          if (out != nullptr)
            return out;
        }
    }
  return nullptr;
}

/**
 * @brief Adds port to composition.
 */
void
Composition::addPort (Port *port)
{
  g_assert_nonnull (port);
  _ports.push_back (port);
  port->initParent (this);
}

/**
 * @brief Gets all ports.
 */
const vector<Port *> *
Composition::getPorts ()
{
  return &_ports;
}

/**
 * @brief Gets port.
 * @param id Port id.
 * @return Port if successful, or null if not found.
 */
Port *
Composition::getPort (const string &id)
{
  for (auto port: _ports)
    if (port->getId () == id)
      return port;
  return nullptr;
}

/**
 * @brief Gets interface mapped by port.
 */
Anchor *
Composition::getMapInterface (Port *port)
{
  Node *node;
  Anchor *iface;

  node = port->getNode ();
  iface = port->getInterface ();
  if (instanceof (Port *, iface))
    {
      Composition *comp = cast (Composition *, node);
      g_assert_nonnull (comp);
      return comp->getMapInterface (cast (Port *, iface));
    }
  else
    {
      return (Anchor *) iface;
    }
}

GINGA_NCL_END
