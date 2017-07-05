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
#include "Connector.h"

GINGA_NCL_BEGIN

Connector::Connector (const string &id) : Entity (id)
{
}

Connector::~Connector ()
{
  _parameters.clear ();
}

Role *
Connector::getRole (const string &roleId)
{
  Role *wRole;
  vector<Role *> *roles = getRoles ();
  if (roles == NULL)
    {
      return NULL;
    }

  vector<Role *>::iterator i;
  for (i = roles->begin (); i != roles->end (); ++i)
    {
      wRole = *i;
      if (wRole->getLabel () == roleId)
        {
          delete roles;
          return wRole;
        }
    }
  delete roles;
  return NULL;
}

void
Connector::addParameter (Parameter *parameter)
{
  g_assert_nonnull (parameter);
  _parameters.push_back (parameter);
}

const vector<Parameter *> *
Connector::getParameters ()
{
  return &_parameters;
}

Parameter *
Connector::getParameter (const string &name)
{
  for (auto param: _parameters)
    if (param->getName () == name)
      return param;
  return nullptr;
}

GINGA_NCL_END
