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
#include "PrivateBase.h"

GINGA_NCL_BEGIN

PrivateBase::PrivateBase (const string &uid) : CompositeNode (uid)
{
  typeSet.insert ("PrivateBase");
}

PrivateBase::~PrivateBase ()
{
  vector<Node *>::iterator i;
  Node *node;

  i = nodes.begin ();
  while (i != nodes.end ())
    {
      node = *i;
      if (node != this && node->getParentComposition () == this)
        {
          delete node;
          node = NULL;
        }
      ++i;
    }
  nodes.clear ();
}

bool
PrivateBase::addNode (Node *node)
{
  if (node == NULL || this->getNode (node->getId ()) != NULL)
    {
      return false;
    }

  nodes.push_back (node);
  node->setParentComposition (this);
  return true;
}

bool
PrivateBase::removeAllNodeOccurrences (const string &nodeUID)
{
  Node *node;

  node = getNode (nodeUID);
  return removeAllNodeOccurrences (node);
}

bool
PrivateBase::removeAllNodeOccurrences (Node *node)
{
  if (node == NULL)
    return false;

  vector<Node *>::iterator iterNode;
  vector<Node *>::iterator i;

  iterNode = nodes.begin ();
  while (iterNode != nodes.end ())
    {
      if ((*(*iterNode)).getId () == node->getId ())
        {
          i = nodes.erase (iterNode);
          if (i == nodes.end ())
            {
              if ((*iterNode)->instanceOf ("ContextNode"))
                {
                  ((ContextNode *)(*iterNode))->removeNode (node);
                }
              return true;
            }
        }
      if ((*iterNode)->instanceOf ("ContextNode"))
        {
          ((ContextNode *)(*iterNode))->removeNode (node);
        }
      ++iterNode;
    }
  return true;
}

GINGA_NCL_END
