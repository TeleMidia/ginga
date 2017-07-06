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
#include "SwitchNode.h"

GINGA_NCL_BEGIN

SwitchNode::SwitchNode (const string &id) : Composition (id)
{
  _defaultNode = nullptr;
}

SwitchNode::~SwitchNode ()
{
  _nodes.clear ();
  _rules.clear ();
}

void
SwitchNode::addNode (Node *node, Rule *rule)
{
  g_assert_nonnull (node);
  g_assert_nonnull (rule);
  _nodes.push_back (node);
  _rules.push_back (rule);
  node->setParent (this);
}

void
SwitchNode::addNode (Node *node)
{
  g_assert_nonnull (node);
  this->setDefaultNode (node);
}

Node *
SwitchNode::getDefaultNode ()
{
  return _defaultNode;
}

Anchor *
SwitchNode::getMapInterface (Port *port)
{
  if (instanceof (SwitchPort *, port))
    return port;
  else
    return Composition::getMapInterface (port);
}

Node *
SwitchNode::getNode (const string &id)
{
  if (_defaultNode != nullptr && _defaultNode->getId () == id)
    return _defaultNode;
  return Composition::getNode (id);
}

const vector <Rule *> *
SwitchNode::getRules ()
{
  return &_rules;
}

void
SwitchNode::setDefaultNode (Node *node)
{
  _defaultNode = node;
}

GINGA_NCL_END
