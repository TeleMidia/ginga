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

#include "Area.h"
#include "CompositeNode.h"
#include "Refer.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new component.
 * @param id Component id.
 */
Node::Node (const string &id) : Entity (id)
{
  _parent = nullptr;
  _lambda = new AreaLambda (id);
  this->addAnchor (_lambda);
}

/**
 * @brief Destroys component.
 */
Node::~Node ()
{
  _anchors.clear ();
  delete _lambda;
}

/**
 * @brief Gets component parent.
 */
CompositeNode *
Node::getParent ()
{
  return _parent;
}

/**
 * @brief Sets component parent.  (Can only be called once.)
 */
void
Node::setParent (CompositeNode *parent)
{
  g_assert_null (_parent);
  g_assert_nonnull (parent);
  _parent = parent;
}

/**
 * @brief Adds anchor to component.
 * @param anchor Anchor.
 */
void
Node::addAnchor (Anchor *anchor)
{
  g_assert_nonnull (anchor);
  _anchors.push_back (anchor);
}

/**
 * @brief Gets all anchors.
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
 */
Anchor *
Node::getLambda ()
{
  g_assert_nonnull (_lambda);
  return _lambda;
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
      perspective = ((CompositeNode *) _parent)->getPerspective ();
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
