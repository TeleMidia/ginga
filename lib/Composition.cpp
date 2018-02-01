/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "Composition.h"

#include "Document.h"

GINGA_NAMESPACE_BEGIN

Composition::Composition (const string &id) : Object (id)
{
}

Composition::~Composition ()
{
}

const set<Object *> *
Composition::getChildren ()
{
  return &_children;
}

Object *
Composition::getChildById (const string &id)
{
  for (auto child : _children)
    if (child->getId () == id)
      return child;
  return nullptr;
}

Object *
Composition::getChildByIdOrAlias (const string &id)
{
  Object *child;
  if ((child = this->getChildById (id)) != nullptr)
    return child;
  for (auto child : _children)
    if (child->hasAlias (id))
      return child;
  return nullptr;
}

void
Composition::addChild (Object *child)
{
  g_assert_nonnull (child);
  if (tryinsert (child, _children, insert))
    {
      child->initParent (this);
      g_assert (_doc->addObject (child));
    }
}

GINGA_NAMESPACE_END
