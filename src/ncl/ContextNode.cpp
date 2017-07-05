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
#include "ContextNode.h"

GINGA_NCL_BEGIN

ContextNode::ContextNode (const string &id) : CompositeNode (id)
{
}

ContextNode::~ContextNode ()
{
  _links.clear ();
}

void
ContextNode::addLink (Link *link)
{
  g_assert_nonnull (link);
  _links.push_back (link);
}

void
ContextNode::addNode (Node *node)
{
  g_assert_nonnull (node);
  _nodes.push_back (node);
  node->setParentComposition (this);
}

const vector<Link *> *
ContextNode::getLinks ()
{
  return &_links;
}

GINGA_NCL_END
