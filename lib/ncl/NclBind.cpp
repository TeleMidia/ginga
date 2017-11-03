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
#include "NclBind.h"

#include "NclComposition.h"

GINGA_NCL_BEGIN

NclBind::NclBind (NclRole *role, NclNode *comp, NclAnchor *iface)
{
  _role = role;
  _node = comp;
  _interface = iface;
}

NclBind::~NclBind ()
{
}

NclRole *
NclBind::getRole ()
{
  return _role;
}

NclNode *
NclBind::getNode ()
{
  return _node;
}

NclAnchor *
NclBind::getInterface ()
{
  return _interface;
}

const map<string, string> *
NclBind::getParameters ()
{
  return &_params;
}

string
NclBind::getParameter (const string &name)
{
  map<string, string>::iterator it;
  return ((it = _params.find (name)) != _params.end ())
    ? it->second : "";
}

void
NclBind::setParameter (const string &name, const string &value)
{
  _params[name] = value;
}

GINGA_NCL_END
