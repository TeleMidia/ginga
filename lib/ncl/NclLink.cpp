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
#include "NclLink.h"

#include "NclPort.h"

GINGA_NAMESPACE_BEGIN

NclLink::NclLink (NclDocument *ncl, const string &id): NclEntity (ncl, id)
{
}

NclLink::~NclLink ()
{
  _binds.clear ();
}

void
NclLink::addBind (NclBind *bind)
{
  g_assert_nonnull (bind);
  _binds.push_back (bind);
}

const list<NclBind *> *
NclLink::getBinds ()
{
  return &_binds;
}

bool
NclLink::contains (NclNode *node, bool condition)
{
  for (auto bind: _binds)
    {
      NclAnchor *iface;
      NclNode *bound;
      NclBind::RoleType roleType;

      roleType = bind->getRoleType ();
      if (roleType == NclBind::CONDITION)
        continue;             // skip

      if (roleType == NclBind::ACTION && condition)
        continue;             // skip

      if ((iface = bind->getInterface ()) != nullptr
          && instanceof (NclPort *, iface))
        {
          cast (NclPort *, iface)->getTarget (&bound, nullptr);
        }
      else
        {
          bound = bind->getNode ();
        }
      g_assert_nonnull (bound);

      if (bound == node)
        return true;            // found
    }
  return false;
}

const map<string, string> *
NclLink::getGhostBinds ()
{
  return &_ghost_binds;
}

string
NclLink::getGhostBind (const string &name)
{
  map<string, string>::iterator it;
  return ((it = _ghost_binds.find (name)) != _ghost_binds.end ())
    ? it->second : "";
}

void
NclLink::setGhostBind (const string &name, const string &value)
{
  _ghost_binds[name] = value;
}

GINGA_NAMESPACE_END
