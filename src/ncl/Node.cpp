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

Node::Node (string id) : Entity (id)
{
  typeSet.insert ("Node");
  parentNode = NULL;
}

Node::~Node ()
{
  vector<Anchor *>::iterator i;
  vector<PropertyAnchor *>::iterator j;

  parentNode = NULL;

  i = anchorList.begin ();
  while (i != anchorList.end ())
    {
      delete *i;
      ++i;
    }

  anchorList.clear ();

  // properties are inside anchorList as well.
  originalPAnchors.clear ();
}

bool
Node::hasProperty (string propName)
{
  vector<PropertyAnchor *>::iterator i;

  i = originalPAnchors.begin ();
  while (i != originalPAnchors.end ())
    {
      if ((*i)->getPropertyName () == propName)
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

      if (!hasProperty (prop->getPropertyName ()))
        {
          cProp = new PropertyAnchor (prop->getPropertyName ());
          cProp->setId (prop->getId ());
          cProp->setPropertyName (prop->getPropertyName ());
          cProp->setPropertyValue (prop->getPropertyValue ());

          originalPAnchors.push_back (cProp);
          anchorList.push_back (cProp);
        }

      ++i;
    }
}

void *
Node::getParentComposition ()
{
  return parentNode;
}

vector<Node *> *
Node::getPerspective ()
{
  vector<Node *> *perspective;

  if (parentNode == NULL)
    {
      perspective = new vector<Node *>;
    }
  else
    {
      perspective = ((CompositeNode *)parentNode)->getPerspective ();
    }
  perspective->push_back ((Node *)this);
  return perspective;
}

void
Node::setParentComposition (void *composition)
{
  if (composition == NULL
      || ((CompositeNode *)composition)->getNode (getId ()) != NULL)
    {
      this->parentNode = (CompositeNode *)composition;
    }
}

bool
Node::addAnchor (int index, Anchor *anchor)
{
  int lSize = (int)anchorList.size ();

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
      anchorList.push_back (anchor);
    }
  else
    {
      anchorList.insert (anchorList.begin () + index, anchor);
    }

  if (anchor->instanceOf ("PropertyAnchor"))
    {
      originalPAnchors.push_back (((PropertyAnchor *)anchor)->clone ());
    }

  return true;
}

bool
Node::addAnchor (Anchor *anchor)
{
  return Node::addAnchor ((int) anchorList.size (), anchor);
}

Anchor *
Node::getAnchor (string anchorId)
{
  vector<Anchor *>::iterator i;
  Anchor *anchor;

  i = anchorList.begin ();
  while (i != anchorList.end ())
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
  int lSize = (int)anchorList.size ();

  if (index < 0 || index > lSize - 1)
    {
      return NULL;
    }
  else
    {
      return (Anchor *)anchorList[index];
    }
}

vector<Anchor *> *
Node::getAnchors ()
{
  return &anchorList;
}

vector<PropertyAnchor *> *
Node::getOriginalPropertyAnchors ()
{
  return &originalPAnchors;
}

PropertyAnchor *
Node::getPropertyAnchor (string propertyName)
{
  vector<Anchor *>::iterator i;
  PropertyAnchor *property;

  i = anchorList.begin ();
  while (i != anchorList.end ())
    {
      if ((*i)->instanceOf ("PropertyAnchor"))
        {
          property = (PropertyAnchor *)(*i);
          if (property->getPropertyName () == propertyName)
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
  return (int) anchorList.size ();
}

int
Node::indexOfAnchor (Anchor *anchor)
{
  vector<Anchor *>::iterator i;
  int n;
  n = 0;

  for (i = anchorList.begin (); i != anchorList.end (); ++i)
    {
      if (*i == anchor)
        {
          return n;
        }
      n++;
    }

  return (int) anchorList.size () + 10;
}

bool
Node::removeAnchor (int index)
{
  if (index < 0 || index >= (int)anchorList.size ())
    {
      return false;
    }

  anchorList.erase (anchorList.begin () + index);
  return true;
}

bool
Node::removeAnchor (Anchor *anchor)
{
  return removeAnchor (indexOfAnchor (anchor));
}

GINGA_NCL_END
