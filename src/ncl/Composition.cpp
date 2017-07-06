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

#include "ginga.h"
#include "Composition.h"

#include "Media.h"
#include "Context.h"
#include "SwitchNode.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCL_BEGIN

Composition::Composition (const string &id) : Node (id)
{
}

Composition::~Composition ()
{
  _nodes.clear ();
  _ports.clear ();
}

void
Composition::addPort (Port *port)
{
  _ports.push_back (port);
}

Port *
Composition::getPort (const string &id)
{
  for (auto port: _ports)
    if (port->getId () == id)
      return port;
  return nullptr;
}

const vector<Port *> *
Composition::getPorts ()
{
  return &_ports;
}

Anchor *
Composition::getMapInterface (Port *port)
{
  Node *node;
  Composition *compositeNode;
  Anchor *interfacePoint;

  node = port->getNode ();
  interfacePoint = port->getInterface ();
  if (instanceof (Port *, interfacePoint))
    {
      compositeNode = cast (Composition *, node);
      g_assert_nonnull (compositeNode);
      return compositeNode->getMapInterface ((Port *)interfacePoint);
    }
  else
    {
      return (Anchor *)interfacePoint;
    }
}

Node *
Composition::getNode (const string &id)
{
  for (auto node: _nodes)
    if (node->getId () == id)
      return node;
  return nullptr;
}

const vector<Node *> *
Composition::getNodes ()
{
  return &_nodes;
}

bool
Composition::recursivelyContainsNode (const string &nodeId)
{
  if (recursivelyGetNode (nodeId) != NULL)
    {
      return true;
    }
  else
    {
      return false;
    }
}

bool
Composition::recursivelyContainsNode (Node *node)
{
  vector<Node *>::iterator it;
  Node *childNode;
  Composition *compositeNode;
  unsigned int i;

  if (_nodes.empty ())
    {
      return false;
    }

  for (i = 0; i < _nodes.size (); i++)
    {
      childNode = _nodes[i];
      if (childNode == node)
        {
          return true;
        }
    }

  for (it = _nodes.begin (); it != _nodes.end (); ++it)
    {
      childNode = (Node *)*it;
      if (instanceof (Composition *, childNode))
        {
          compositeNode = (Composition *)childNode;
          if (compositeNode->recursivelyContainsNode (node))
            {
              return true;
            }
        }
      else if (instanceof (Refer *, childNode))
        {
          childNode = childNode->derefer ();
          if (childNode == node)
            {
              return true;
            }
          else if (instanceof (Composition *, childNode))
            {
              compositeNode = (Composition *)childNode;
              if (compositeNode->recursivelyContainsNode (node))
                {
                  return true;
                }
            }
        }
    }
  return false;
}

Node *
Composition::recursivelyGetNode (const string &nodeId)
{
  Node *node;
  vector<Node *>::iterator i;

  if (_nodes.empty ())
    {
      return NULL;
    }

  for (i = _nodes.begin (); i != _nodes.end (); ++i)
    {
      if (((*i)->getId ()).compare (nodeId) == 0)
        {
          return (*i);
        }

      if (instanceof (Composition *, (*i)))
        {
          node = ((Composition *)(*i))->recursivelyGetNode (nodeId);
          if (node != NULL)
            {
              return node;
            }
        }
    }

  return NULL;
}

GINGA_NCL_END
