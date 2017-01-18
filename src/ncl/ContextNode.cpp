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

#include "config.h"
#include "ContextNode.h"

GINGA_NCL_BEGIN

ContextNode::ContextNode (string id) : CompositeNode (id)
{

  typeSet.insert ("ContextNode");
  typeSet.insert ("DocumentNode");
  typeSet.insert ("LinkComposition");
}

ContextNode::~ContextNode ()
{
  set<Link *>::iterator i;
  vector<Node *>::iterator j;
  Node *node;

  descriptorCollection.clear ();

  i = linkSet.begin ();
  while (i != linkSet.end ())
    {
      delete *i;
      ++i;
    }
  linkSet.clear ();

  j = nodes.begin ();
  while (j != nodes.end ())
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

  nodes.clear ();
}

bool
ContextNode::addLink (Link *link)
{
  if (link == NULL)
    return false;

  linkSet.insert (link);
  link->setParentComposition (this);
  return true;
}

bool
ContextNode::addNode (Node *node)
{
  if (!node->instanceOf ("DocumentNode"))
    {
      return false;
    }

  if (node == NULL || this->getNode (node->getId ()) != NULL)
    {
      return false;
    }

  nodes.push_back (node);
  node->setParentComposition (this);
  return true;
}

void
ContextNode::clearLinks ()
{
  set<Link *>::iterator it;
  Link *link;

  for (it = linkSet.begin (); it != linkSet.end (); ++it)
    {
      link = (Link *)(*it);
      link->setParentComposition (NULL);
    }
  linkSet.clear ();
}

bool
ContextNode::containsLink (Link *link)
{
  set<Link *>::iterator i;

  i = linkSet.find (link);
  if (i != linkSet.end ())
    {
      return true;
    }
  return false;
}

set<Link *> *
ContextNode::getLinks ()
{
  if (this->linkSet.size () == 0)
    {
      return NULL;
    }

  return &linkSet;
}

Link *
ContextNode::getLink (string linkId)
{
  set<Link *>::iterator i;

  i = linkSet.begin ();
  while (i != linkSet.end ())
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
  if (descriptorCollection.count (node->getId ()) != 0)
    {
      return descriptorCollection[node->getId ()];
    }

  return NULL;
}

int
ContextNode::getNumLinks ()
{
  return (int)(linkSet.size ());
}

bool
ContextNode::removeLink (Link *link)
{
  set<Link *>::iterator it;

  it = linkSet.find (link);
  if (it != linkSet.end ())
    {
      linkSet.erase (it);
      link->setParentComposition (NULL);
      return true;
    }
  return false;
}

bool
ContextNode::setNodeDescriptor (string nodeId,
                                GenericDescriptor *descriptor)
{

  Node *node;
  node = getNode (nodeId);
  if (node == NULL)
    {
      return false;
    }

  if (descriptorCollection.count (nodeId) != 0)
    {
      if (descriptor == NULL)
        {
          descriptorCollection.erase (descriptorCollection.find (nodeId));

          return true;
        }
    }
  else if (descriptor == NULL)
    {
      return true;
    }

  descriptorCollection[nodeId] = descriptor;
  return true;
}

GINGA_NCL_END
