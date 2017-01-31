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

NclNodeNesting::NclNodeNesting () { initialize (); }

NclNodeNesting::NclNodeNesting (Node *node)
{
  initialize ();
  insertAnchorNode (node);
}

NclNodeNesting::NclNodeNesting (NclNodeNesting *seq)
{
  initialize ();
  append (seq);
}

NclNodeNesting::NclNodeNesting (vector<Node *> *seq)
{
  initialize ();
  append (seq);
}

NclNodeNesting::~NclNodeNesting ()
{
  Thread::mutexLock (&mutexNodes);
  if (nodes != NULL)
    {
      delete nodes;
      nodes = NULL;
    }
  Thread::mutexUnlock (&mutexNodes);
  Thread::mutexDestroy (&mutexNodes);
}

void
NclNodeNesting::initialize ()
{
  string type = "NclNodeNesting";
  this->nodes = new vector<Node *>;
  id = "";
  typeSet.insert (type);
  Thread::mutexInit (&mutexNodes, false);
}

bool
NclNodeNesting::instanceOf (const string &s)
{
  if (!typeSet.empty ())
    {
      return (typeSet.find (s) != typeSet.end ());
    }
  else
    {
      return false;
    }
}

void
NclNodeNesting::append (NclNodeNesting *otherSeq)
{
  int i, size;
  Node *node;

  size = otherSeq->getNumNodes ();
  for (i = 0; i < size; i++)
    {
      node = otherSeq->getNode (i);
      insertAnchorNode (node);
    }
}

void
NclNodeNesting::append (vector<Node *> *otherSeq)
{
  vector<Node *>::iterator i;

  for (i = otherSeq->begin (); i != otherSeq->end (); ++i)
    {
      insertAnchorNode (*i);
    }
}

Node *
NclNodeNesting::getAnchorNode ()
{
  Node *node;

  Thread::mutexLock (&mutexNodes);
  if (nodes == NULL || nodes->empty ())
    {
      Thread::mutexUnlock (&mutexNodes);
      return NULL;
    }
  else if (nodes->size () == 1)
    {
      node = *(nodes->begin ());
      Thread::mutexUnlock (&mutexNodes);
      return node;
    }
  else
    {
      node = *(nodes->end () - 1);
      Thread::mutexUnlock (&mutexNodes);
      return node;
    }
}

Node *
NclNodeNesting::getHeadNode ()
{
  Node *node;

  Thread::mutexLock (&mutexNodes);
  if (nodes == NULL || nodes->empty ())
    {
      Thread::mutexUnlock (&mutexNodes);
      return NULL;
    }
  else
    {
      node = *(nodes->begin ());
      Thread::mutexUnlock (&mutexNodes);
      return node;
    }
}

Node *
NclNodeNesting::getNode (int index)
{
  Node *node;
  vector<Node *>::iterator i;

  Thread::mutexLock (&mutexNodes);

  if (nodes == NULL || nodes->empty () || index < 0
      || index >= (int)(nodes->size ()))
    {
      Thread::mutexUnlock (&mutexNodes);
      return NULL;
    }

  i = nodes->begin () + index;
  node = *i;
  Thread::mutexUnlock (&mutexNodes);

  return node;
}

int
NclNodeNesting::getNumNodes ()
{
  int s;

  Thread::mutexLock (&mutexNodes);
  if (nodes == NULL)
    {
      s = 0;
    }
  else
    {
      s = (int) nodes->size ();
    }
  Thread::mutexUnlock (&mutexNodes);
  return s;
}

void
NclNodeNesting::insertAnchorNode (Node *node)
{
  string nodeId;

  Thread::mutexLock (&mutexNodes);
  if (nodes == NULL)
    {
      Thread::mutexUnlock (&mutexNodes);
      return;
    }

  nodeId = node->getId ();
  if (nodes->size () > 0)
    {
      id = id + SystemCompat::getIUriD () + nodeId;
    }
  else
    {
      id = nodeId;
    }

  nodes->push_back (node);
  Thread::mutexUnlock (&mutexNodes);
}

void
NclNodeNesting::insertHeadNode (Node *node)
{
  Thread::mutexLock (&mutexNodes);
  if (nodes == NULL)
    {
      Thread::mutexUnlock (&mutexNodes);
      return;
    }

  if (nodes->size () > 0)
    {
      id = node->getId () + SystemCompat::getIUriD () + id;
    }
  else
    {
      id = node->getId ();
    }
  nodes->insert (nodes->begin (), node);

  Thread::mutexUnlock (&mutexNodes);
}

bool
NclNodeNesting::removeAnchorNode ()
{
  Thread::mutexLock (&mutexNodes);

  if (nodes == NULL || nodes->empty ())
    {
      Thread::mutexUnlock (&mutexNodes);
      return false;
    }

  nodes->erase (nodes->end () - 1);
  Thread::mutexUnlock (&mutexNodes);

  if (id.find (SystemCompat::getIUriD ()) != std::string::npos)
    {
      id = id.substr (0, id.find_last_of (SystemCompat::getIUriD ()));
    }

  return true;
}

bool
NclNodeNesting::removeHeadNode ()
{
  Thread::mutexLock (&mutexNodes);

  if (nodes == NULL || nodes->empty ())
    {
      Thread::mutexUnlock (&mutexNodes);
      return false;
    }

  nodes->erase (nodes->begin ());
  Thread::mutexUnlock (&mutexNodes);

  if (id.find (SystemCompat::getIUriD ()) != std::string::npos)
    {
      id = id.substr (0, id.find_last_of (SystemCompat::getIUriD ()));
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

string
NclNodeNesting::toString ()
{
  return id;
}

GINGA_FORMATTER_END
