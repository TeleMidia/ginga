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

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_NCL_BEGIN

Node::Node (const string &id) : Entity (id)
{
  _parentNode = NULL;
}

Node::~Node ()
{
  vector<Anchor *>::iterator i;
  vector<PropertyAnchor *>::iterator j;

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
  vector<PropertyAnchor *>::iterator i;

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

void
Node::copyProperties (Node *node)
{
  vector<PropertyAnchor *> *props;
  vector<PropertyAnchor *>::iterator i;
  PropertyAnchor *prop;
  PropertyAnchor *cProp;

  props = node->getOriginalPropertyAnchors ();

  i = props->begin ();
  while (i != props->end ())
    {
      prop = *i;

      if (!hasProperty (prop->getName ()))
        {
          cProp = new PropertyAnchor (prop->getName ());
          cProp->setId (prop->getId ());
          cProp->setName (prop->getName ());
          cProp->setValue (prop->getValue ());

          _originalPAnchors.push_back (cProp);
          _anchorList.push_back (cProp);
        }

      ++i;
    }
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

  if (instanceof (PropertyAnchor *, anchor))
    {
      _originalPAnchors.push_back (((PropertyAnchor *)anchor)->clone ());
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
  int lSize = (int)_anchorList.size ();

  if (index < 0 || index > lSize - 1)
    {
      return NULL;
    }
  else
    {
      return (Anchor *)_anchorList[index];
    }
}

const vector<Anchor *> &
Node::getAnchors ()
{
  return this->_anchorList;
}

vector<PropertyAnchor *> *
Node::getOriginalPropertyAnchors ()
{
  return &_originalPAnchors;
}

PropertyAnchor *
Node::getPropertyAnchor (const string &propertyName)
{
  vector<Anchor *>::iterator i;
  PropertyAnchor *property;

  i = _anchorList.begin ();
  while (i != _anchorList.end ())
    {
      if (instanceof (PropertyAnchor *, (*i)))
        {
          property = (PropertyAnchor *)(*i);
          if (property->getName () == propertyName)
            {
              return property;
            }
        }
      ++i;
    }

  return NULL;
}

int
Node::getNumAnchors ()
{
  return (int) _anchorList.size ();
}

int
Node::indexOfAnchor (Anchor *anchor)
{
  vector<Anchor *>::iterator i;
  int n;
  n = 0;

  for (i = _anchorList.begin (); i != _anchorList.end (); ++i)
    {
      if (*i == anchor)
        {
          return n;
        }
      n++;
    }

  return (int) _anchorList.size () + 10;
}

bool
Node::removeAnchor (int index)
{
  if (index < 0 || index >= (int)_anchorList.size ())
    {
      return false;
    }

  _anchorList.erase (_anchorList.begin () + index);
  return true;
}

bool
Node::removeAnchor (Anchor *anchor)
{
  return removeAnchor (indexOfAnchor (anchor));
}

GINGA_NCL_END
