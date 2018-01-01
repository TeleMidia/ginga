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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "NclPort.h"

GINGA_NAMESPACE_BEGIN


// Public.

NclPort::NclPort (NclDocument *ncl, const string &id): NclAnchor (ncl, id)
{
  _parent = nullptr;
  _node = nullptr;
  _interface = nullptr;
}

NclPort::~NclPort ()
{
}

NclComposition *
NclPort::getParent ()
{
  return _parent;
}

void
NclPort::initParent (NclComposition *parent)
{
  g_assert_null (_parent);
  g_assert_nonnull (parent);
  _parent = parent;
}

NclNode *
NclPort::getNode ()
{
  return _node;
}

void
NclPort::initNode (NclNode *node)
{
  g_assert_null (_node);
  g_assert_nonnull (node);
  _node = node;
}

NclAnchor *
NclPort::getInterface ()
{
  return _interface;
}

void
NclPort::initInterface (NclAnchor *iface)
{
  g_assert_null (_interface);
  g_assert_nonnull (iface);
  _interface = iface;
}

void
NclPort::getTarget (NclNode **node, NclAnchor **iface)
{
  if (node != nullptr)
    {
      NclNode *target = this->getFinalNode ();
      g_assert_nonnull (target);
      *node = target;
    }
  if (iface != nullptr)
    {
      NclAnchor *target = this->getFinalInterface ();
      g_assert_nonnull (target);
      *iface = target;
    }
}


// Private.

NclNode *
NclPort::getFinalNode ()
{
  if (instanceof (NclPort *, _interface))
    return cast (NclPort *, _interface)->getFinalNode ();
  else
    return _node;
}

NclAnchor *
NclPort::getFinalInterface ()
{
  if (instanceof (NclPort *, _interface))
    return cast (NclPort *, _interface)->getFinalInterface ();
  else
    return _interface;
}

GINGA_NAMESPACE_END
