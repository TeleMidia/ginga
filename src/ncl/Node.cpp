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
#include "Node.h"
#include "CompositeNode.h"

GINGA_NCL_BEGIN

Node::Node (const string &id) : Entity (id)
{
  _parentNode = nullptr;
}

Node::~Node ()
{
  vector<Anchor *>::iterator i;
  vector<Property *>::iterator j;

  _parentNode = NULL;

  i = _anchorList.begin ();
  while (i != _anchorList.end ())
    {
      delete *i;
      ++i;
    }

  _anchorList.clear ();

  // properties are inside anchorList as well.
  _originalPAnchors.clear ();
}

bool
Node::hasProperty (const string &propName)
{
  vector<Property *>::iterator i;

  i = _originalPAnchors.begin ();
  while (i != _originalPAnchors.end ())
    {
      if ((*i)->getName () == propName)
        {
          return true;
        }
      ++i;
    }

  return false;
}

CompositeNode *
Node::getParentComposition ()
{
  return _parentNode;
}

vector<Node *> *
Node::getPerspective ()
{
  vector<Node *> *perspective;

  if (_parentNode == NULL)
    {
      perspective = new vector<Node *>;
    }
  else
    {
      perspective = ((CompositeNode *)_parentNode)->getPerspective ();
    }
  perspective->push_back ((Node *)this);
  return perspective;
}

void
Node::setParentComposition (CompositeNode *composition)
{
  if (composition == NULL
      || composition->getNode (getId ()) != NULL)
    {
      this->_parentNode = composition;
    }
}

bool
Node::addAnchor (int index, Anchor *anchor)
{
  int lSize = (int)_anchorList.size ();

  // anchor position must be in the correct range and anchor must exist
  if ((index < 0 || index > lSize) || anchor == NULL)
    {
      return false;
    }

  // anchor id must be unique - conflicts with referredNode anchor ids
  // can only be solved at runtime, since anchors can be inserted after
  if (getAnchor (anchor->getId ()) != NULL)
    {
      return false;
    }

  if (index == lSize)
    {
      _anchorList.push_back (anchor);
    }
  else
    {
      _anchorList.insert (_anchorList.begin () + index, anchor);
    }

  if (instanceof (Property *, anchor))
    {
      // g_assert_not_reached ();
      // _originalPAnchors.push_back (((Property *)anchor)->clone ());
    }

  return true;
}

bool
Node::addAnchor (Anchor *anchor)
{
  return Node::addAnchor ((int) _anchorList.size (), anchor);
}

Anchor *
Node::getAnchor (const string &anchorId)
{
  vector<Anchor *>::iterator i;
  Anchor *anchor;

  i = _anchorList.begin ();
  while (i != _anchorList.end ())
    {
      anchor = *i;
      if (anchor == NULL)
        {
          return NULL;
        }
      else if (anchor->getId () != "" && anchor->getId () == anchorId)
        {
          return anchor;
        }
      ++i;
    }
  return NULL;
}

Anchor *
Node::getAnchor (int index)
{
  return (Anchor *)_anchorList[(size_t)index];
}

const vector<Anchor *> &
Node::getAnchors ()
{
  return this->_anchorList;
}

Property *
Node::getProperty (const string &propertyName)
{
  vector<Anchor *>::iterator i;
  Property *property;

  i = _anchorList.begin ();
  while (i != _anchorList.end ())
    {
      if (instanceof (Property *, (*i)))
        {
          property = (Property *)(*i);
          if (property->getName () == propertyName)
            {
              return property;
            }
        }
      ++i;
    }

  return NULL;
}

GINGA_NCL_END
