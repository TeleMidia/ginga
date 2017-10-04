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
#include "Node.h"

#include "Area.h"
#include "Composition.h"
#include "Property.h"
#include "Refer.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new node.
 * @param ncl Parent document.
 * @param id Node id.
 */
Node::Node (NclDocument *ncl, const string &id) : Entity (ncl, id)
{
  _parent = nullptr;
  _lambda = new AreaLambda (ncl, id + "@lambda");
  this->addAnchor (_lambda);
}

/**
 * @brief Destroys node.
 */
Node::~Node ()
{
  for (auto anchor: _anchors)
    delete anchor;
}

/**
 * @brief Gets node parent.
 * @return Parent composition.
 */
Composition *
Node::getParent ()
{
  return _parent;
}

/**
 * @brief Initializes node parent.
 * @param parent Parent composition.
 */
void
Node::initParent (Composition *parent)
{
  g_assert_null (_parent);
  g_assert_nonnull (parent);
  _parent = parent;
}

/**
 * @brief Adds anchor to node.
 * @param anchor Anchor.
 */
void
Node::addAnchor (Anchor *anchor)
{
  g_assert_nonnull (anchor);
  _anchors.push_back (anchor);
}

/**
 * @brief Gets node anchors.
 * @return Node anchors.
 */
const vector<Anchor *> *
Node::getAnchors ()
{
  return &_anchors;
}

/**
 * @brief Gets anchor.
 * @param id Anchor id.
 * @return Anchor if successful, or null if not found.
 */
Anchor *
Node::getAnchor (const string &id)
{
  for (auto anchor: _anchors)
    if (anchor->getId () == id)
      return anchor;
  return nullptr;
}

/**
 * @brief Gets lambda anchor.
 * @return Lambda anchor.
 */
Anchor *
Node::getLambda ()
{
  g_assert_nonnull (_lambda);
  return _lambda;
}

/**
 * @brief Tests whether node has property anchor.
 * @param name Property name.
 * @return True if successful, or false otherwise.
 */
bool
Node::hasProperty (const string &name)
{
  for (auto anchor: _anchors)
    {
      Property *prop = cast (Property *, anchor);
      if (prop == nullptr)
        continue;
      if (prop->getId () == name)
        return true;
    }
  return false;
}

/**
 * @brief Gets property anchor.
 * @param name Property name.
 * @return Property value.
 */
string
Node::getProperty (const string &name)
{
  for (auto anchor: _anchors)
    {
      Property *prop = cast (Property *, anchor);
      if (prop == nullptr)
        continue;
      if (prop->getId () != name)
        continue;
      return prop->getValue ();
    }
  return "";
}

/**
 * @brief Sets property anchor.  Creates it if it doesn't exist.
 * @param name Property name.
 * @param value Property value.
 */
void
Node::setProperty (const string &name, const string &value)
{
  Property *prop;
  for (auto anchor: _anchors)
    {
      prop = cast (Property *, anchor);
      if (prop == nullptr)
        continue;
      if (prop->getId () == name)
        goto tail;
    }
  prop = new Property (this->getDocument (), name);
  this->addAnchor (prop);
 tail:
  prop->setValue (value);
}


// FIXME: Remove this with NclNodeNesting stuff.

vector<Node *> *
Node::getPerspective ()
{
  vector<Node *> *perspective;

  if (_parent == nullptr)
    {
      perspective = new vector<Node *>;
    }
  else
    {
      perspective = ((Composition *) _parent)->getPerspective ();
    }
  perspective->push_back (this);
  return perspective;
}


// FIXME: Find a better place to put this.

Node *
Node::derefer ()
{
  if (instanceof (Refer *, this))
    {
      Node *result;
      result = cast (Node *, cast (Refer *, this)->getReferred ());
      g_assert_nonnull (result);
      return result;
    }
  return this;
}

GINGA_NCL_END
