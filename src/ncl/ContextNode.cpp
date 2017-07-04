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
  set<Link *>::iterator i;
  vector<Node *>::iterator j;
  Node *node;

  _descriptorCollection.clear ();

  i = _linkSet.begin ();
  while (i != _linkSet.end ())
    {
      delete *i;
      ++i;
    }
  _linkSet.clear ();

  j = _nodes.begin ();
  while (j != _nodes.end ())
    {
      node = (*j);
      if (Entity::hasInstance (node, true))
        {
          if (node != this && node->getParentComposition () == this)
            {
              (*j)->setParentComposition (NULL);
              delete *j;
            }
        }
      ++j;
    }

  _nodes.clear ();
}

bool
ContextNode::addLink (Link *link)
{
  if (link == NULL)
    return false;

  _linkSet.insert (link);
  link->setParentComposition (this);
  return true;
}

bool
ContextNode::addNode (Node *node)
{
  if (!isDocumentNode(node))
    {
      return false;
    }

  if (node == NULL || this->getNode (node->getId ()) != NULL)
    {
      return false;
    }

  _nodes.push_back (node);
  node->setParentComposition (this);
  return true;
}

void
ContextNode::clearLinks ()
{
  set<Link *>::iterator it;
  Link *link;

  for (it = _linkSet.begin (); it != _linkSet.end (); ++it)
    {
      link = (Link *)(*it);
      link->setParentComposition (NULL);
    }
  _linkSet.clear ();
}

bool
ContextNode::containsLink (Link *link)
{
  set<Link *>::iterator i;

  i = _linkSet.find (link);
  if (i != _linkSet.end ())
    {
      return true;
    }
  return false;
}

set<Link *> *
ContextNode::getLinks ()
{
  return &_linkSet;
}

Link *
ContextNode::getLink (const string &linkId)
{
  set<Link *>::iterator i;

  i = _linkSet.begin ();
  while (i != _linkSet.end ())
    {
      if ((*i)->getId () != "" && (*i)->getId () == linkId)
        {
          return *i;
        }
      ++i;
    }
  return NULL;
}

GenericDescriptor *
ContextNode::getNodeDescriptor (Node *node)
{
  if (_descriptorCollection.count (node->getId ()) != 0)
    {
      return _descriptorCollection[node->getId ()];
    }

  return NULL;
}

int
ContextNode::getNumLinks ()
{
  return (int)(_linkSet.size ());
}

bool
ContextNode::removeLink (Link *link)
{
  set<Link *>::iterator it;

  it = _linkSet.find (link);
  if (it != _linkSet.end ())
    {
      _linkSet.erase (it);
      link->setParentComposition (NULL);
      return true;
    }
  return false;
}

bool
ContextNode::setNodeDescriptor (const string &nodeId,
                                GenericDescriptor *descriptor)
{
  Node *node;
  node = getNode (nodeId);
  if (node == NULL)
    {
      return false;
    }

  if (_descriptorCollection.count (nodeId) != 0)
    {
      if (descriptor == NULL)
        {
          _descriptorCollection.erase (_descriptorCollection.find (nodeId));

          return true;
        }
    }
  else if (descriptor == NULL)
    {
      return true;
    }

  _descriptorCollection[nodeId] = descriptor;
  return true;
}

GINGA_NCL_END
