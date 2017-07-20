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
#include "NclNodeNesting.h"

GINGA_FORMATTER_BEGIN

NclNodeNesting::NclNodeNesting ()
{
  id = "";
}

NclNodeNesting::NclNodeNesting (Node *node)
{
  id = "";
  insertAnchorNode (node);
}

NclNodeNesting::NclNodeNesting (NclNodeNesting *seq)
{
  id = "";
  append (seq);
}

NclNodeNesting::NclNodeNesting (vector<Node *> *seq)
{
  id = "";
  append (seq);
}

void
NclNodeNesting::append (NclNodeNesting *otherSeq)
{
  for (int i = 0; i < otherSeq->getNumNodes (); i++)
    {
      Node *node = otherSeq->getNode (i);
      insertAnchorNode (node);
    }
}

void
NclNodeNesting::append (vector<Node *> *otherSeq)
{
  for (Node *node : *otherSeq)
    {
      insertAnchorNode (node);
    }
}

Node *
NclNodeNesting::getAnchorNode ()
{
  Node *node;

  if (nodes.empty ())
    {
      return nullptr;
    }
  else if (nodes.size () == 1)
    {
      node = *(nodes.begin ());
      return node;
    }
  else
    {
      node = *(nodes.end () - 1);
      return node;
    }
}

Node *
NclNodeNesting::getHeadNode ()
{
  Node *node;

  if (nodes.empty ())
    {
      return nullptr;
    }
  else
    {
      node = *(nodes.begin ());
      return node;
    }
}

Node *
NclNodeNesting::getNode (int index)
{
  Node *node;
  vector<Node *>::iterator i;

  if (nodes.empty () || index < 0
      || index >= (int)(nodes.size ()))
    {
      return nullptr;
    }

  i = nodes.begin () + index;
  node = *i;
  return node;
}

int
NclNodeNesting::getNumNodes ()
{
  return (int) nodes.size ();
}

void
NclNodeNesting::insertAnchorNode (Node *node)
{
  string nodeId;

  nodeId = node->getId ();
  if (nodes.size () > 0)
    {
      id = id + "/" + nodeId;
    }
  else
    {
      id = nodeId;
    }

  nodes.push_back (node);
}

void
NclNodeNesting::insertHeadNode (Node *node)
{
  if (nodes.size () > 0)
    {
      id = node->getId () + "/" + id;
    }
  else
    {
      id = node->getId ();
    }

  nodes.insert (nodes.begin (), node);
}

bool
NclNodeNesting::removeAnchorNode ()
{
  if (nodes.empty ())
    return false;

  nodes.erase (nodes.end () - 1);

  if (id.find ("/") != std::string::npos)
    {
      id = id.substr (0, id.find_last_of ("/"));
    }

  return true;
}

bool
NclNodeNesting::removeHeadNode ()
{
  if (nodes.empty ())
    {
      return false;
    }

  nodes.erase (nodes.begin ());

  if (id.find ("/") != std::string::npos)
    {
      id = id.substr (0, id.find_last_of ("/"));
    }

  return true;
}

string
NclNodeNesting::getId ()
{
  return id;
}

NclNodeNesting *
NclNodeNesting::copy ()
{
  return new NclNodeNesting (this);
}

GINGA_FORMATTER_END
