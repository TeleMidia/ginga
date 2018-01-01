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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "NclComposition.h"

GINGA_NAMESPACE_BEGIN

NclComposition::NclComposition (NclDocument *ncl, const string &id)
  :NclNode (ncl, id)
{
}

NclComposition::~NclComposition ()
{
  for (auto node: _nodes)
    delete node;
  for (auto port: _ports)
    delete port;
  _nodes.clear ();
  _ports.clear ();
}

void
NclComposition::addNode (NclNode *node)
{
  g_assert_nonnull (node);
  _nodes.push_back (node);
  node->initParent (this);
}

const list<NclNode *> *
NclComposition::getNodes ()
{
  return &_nodes;
}

NclNode *
NclComposition::getNode (const string &id)
{
  for (auto node: _nodes)
    if (node->getId () == id)
      return node;
  return nullptr;
}

NclNode *
NclComposition::getNestedNode (const string &id)
{
  for (auto node: _nodes)
    {
      if (node->getId () == id)
        return node;
      if (instanceof (NclComposition *, node))
        {
          NclNode *out = cast (NclComposition *, node)->getNestedNode (id);
          if (out != nullptr)
            return out;
        }
    }
  return nullptr;
}

void
NclComposition::addPort (NclPort *port)
{
  g_assert_nonnull (port);
  _ports.push_back (port);
  port->initParent (this);
}

const list<NclPort *> *
NclComposition::getPorts ()
{
  return &_ports;
}

NclPort *
NclComposition::getPort (const string &id)
{
  for (auto port: _ports)
    if (port->getId () == id)
      return port;
  return nullptr;
}

NclAnchor *
NclComposition::getMapInterface (NclPort *port)
{
  NclNode *node;
  NclAnchor *iface;

  node = port->getNode ();
  iface = port->getInterface ();
  if (instanceof (NclPort *, iface))
    {
      NclComposition *comp = cast (NclComposition *, node);
      g_assert_nonnull (comp);
      return comp->getMapInterface (cast (NclPort *, iface));
    }
  else
    {
      return (NclAnchor *) iface;
    }
}

GINGA_NAMESPACE_END
