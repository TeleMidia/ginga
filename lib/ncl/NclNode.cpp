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
#include "NclNode.h"

#include "NclArea.h"
#include "NclComposition.h"
#include "NclProperty.h"
#include "NclMediaRefer.h"

GINGA_NCL_BEGIN

NclNode::NclNode (NclDocument *ncl, const string &id): NclEntity (ncl, id)
{
  _parent = nullptr;
  _lambda = new NclAreaLambda (ncl, id + "@lambda");
  this->addAnchor (_lambda);
}

NclNode::~NclNode ()
{
  for (auto anchor: _anchors)
    delete anchor;
}

NclComposition *
NclNode::getParent ()
{
  return _parent;
}

void
NclNode::initParent (NclComposition *parent)
{
  g_assert_null (_parent);
  g_assert_nonnull (parent);
  _parent = parent;
}

void
NclNode::addAnchor (NclAnchor *anchor)
{
  g_assert_nonnull (anchor);
  _anchors.push_back (anchor);
}

const vector<NclAnchor *> *
NclNode::getAnchors ()
{
  return &_anchors;
}

NclAnchor *
NclNode::getAnchor (const string &id)
{
  for (auto anchor: _anchors)
    if (anchor->getId () == id)
      return anchor;
  return nullptr;
}

NclAnchor *
NclNode::getLambda ()
{
  g_assert_nonnull (_lambda);
  return _lambda;
}

bool
NclNode::hasProperty (const string &name)
{
  for (auto anchor: _anchors)
    {
      NclProperty *prop = cast (NclProperty *, anchor);
      if (prop == nullptr)
        continue;
      if (prop->getId () == name)
        return true;
    }
  return false;
}

string
NclNode::getProperty (const string &name)
{
  for (auto anchor: _anchors)
    {
      NclProperty *prop = cast (NclProperty *, anchor);
      if (prop == nullptr)
        continue;
      if (prop->getId () != name)
        continue;
      return prop->getValue ();
    }
  return "";
}

void
NclNode::setProperty (const string &name, const string &value)
{
  NclProperty *prop;
  for (auto anchor: _anchors)
    {
      prop = cast (NclProperty *, anchor);
      if (prop == nullptr)
        continue;
      if (prop->getId () == name)
        goto tail;
    }
  prop = new NclProperty (this->getDocument (), name);
  this->addAnchor (prop);
 tail:
  prop->setValue (value);
}

NclNode *
NclNode::derefer ()
{
  if (instanceof (NclMediaRefer *, this))
    {
      NclNode *result;
      result = cast (NclNode *, cast (NclMediaRefer *, this)
                     ->getReferred ());
      g_assert_nonnull (result);
      return result;
    }
  return this;
}

GINGA_NCL_END
