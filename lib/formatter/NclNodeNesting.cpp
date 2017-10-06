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
#include "NclNodeNesting.h"

GINGA_FORMATTER_BEGIN

NclNodeNesting::NclNodeNesting ()
{
  _id = "";
}

NclNodeNesting::NclNodeNesting (Node *node)
{
  _id = "";
  insertAnchorNode (node);
}

NclNodeNesting::NclNodeNesting (NclNodeNesting *seq)
{
  _id = "";
  append (seq);
}

NclNodeNesting::NclNodeNesting (vector<Node *> *seq)
{
  _id = "";
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

  if (_nodes.empty ())
    {
      return nullptr;
    }
  else if (_nodes.size () == 1)
    {
      node = *(_nodes.begin ());
      return node;
    }
  else
    {
      node = *(_nodes.end () - 1);
      return node;
    }
}

Node *
NclNodeNesting::getHeadNode ()
{
  Node *node;

  if (_nodes.empty ())
    {
      return nullptr;
    }
  else
    {
      node = *(_nodes.begin ());
      return node;
    }
}

Node *
NclNodeNesting::getNode (int index)
{
  Node *node;
  vector<Node *>::iterator i;

  if (_nodes.empty () || index < 0
      || index >= (int)(_nodes.size ()))
    {
      return nullptr;
    }

  i = _nodes.begin () + index;
  node = *i;
  return node;
}

int
NclNodeNesting::getNumNodes ()
{
  return (int) _nodes.size ();
}

void
NclNodeNesting::insertAnchorNode (Node *node)
{
  string nodeId;

  nodeId = node->getId ();
  if (_nodes.size () > 0)
    {
      _id = _id + "/" + nodeId;
    }
  else
    {
      _id = nodeId;
    }

  _nodes.push_back (node);
}

void
NclNodeNesting::insertHeadNode (Node *node)
{
  if (_nodes.size () > 0)
    {
      _id = node->getId () + "/" + _id;
    }
  else
    {
      _id = node->getId ();
    }

  _nodes.insert (_nodes.begin (), node);
}

bool
NclNodeNesting::removeAnchorNode ()
{
  if (_nodes.empty ())
    return false;

  _nodes.erase (_nodes.end () - 1);

  if (_id.find ("/") != std::string::npos)
    {
      _id = _id.substr (0, _id.find_last_of ("/"));
    }

  return true;
}

bool
NclNodeNesting::removeHeadNode ()
{
  if (_nodes.empty ())
    {
      return false;
    }

  _nodes.erase (_nodes.begin ());

  if (_id.find ("/") != std::string::npos)
    {
      _id = _id.substr (0, _id.find_last_of ("/"));
    }

  return true;
}

string
NclNodeNesting::getId ()
{
  return _id;
}

NclNodeNesting *
NclNodeNesting::copy ()
{
  return new NclNodeNesting (this);
}

GINGA_FORMATTER_END
