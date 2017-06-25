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
  _params = new map<string, Parameter *>;
}

Connector::~Connector ()
{
  map<string, Parameter *>::iterator i;

  if (_params != NULL)
    {
      i = _params->begin ();
      while (i != _params->end ())
        {
          delete i->second;
          ++i;
        }
      delete _params;
      _params = NULL;
    }
}

int
Connector::getNumRoles ()
{
  int numOfRoles;

  vector<Role *> *childRoles;
  childRoles = getRoles ();
  numOfRoles = (int) childRoles->size ();
  delete childRoles;

  return numOfRoles;
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
  if (parameter == NULL)
    return;

  map<string, Parameter *>::iterator i;
  for (i = _params->begin (); i != _params->end (); ++i)
    if (i->first == parameter->getName ())
      return;

  (*_params)[parameter->getName ()] = parameter;
}

vector<Parameter *> *
Connector::getParameters ()
{
  if (_params->empty ())
    return NULL;

  vector<Parameter *> *params;
  params = new vector<Parameter *>;
  map<string, Parameter *>::iterator i;
  for (i = _params->begin (); i != _params->end (); ++i)
    params->push_back (i->second);

  return params;
}

Parameter *
Connector::getParameter (const string &name)
{
  if (_params->empty ())
    return NULL;

  map<string, Parameter *>::iterator i;
  for (i = _params->begin (); i != _params->end (); ++i)
    if (i->first == name)
      return (Parameter *)(i->second);

  return NULL;
}

bool
Connector::removeParameter (const string &name)
{
  if (_params->empty ())
    return false;

  map<string, Parameter *>::iterator i;
  for (i = _params->begin (); i != _params->end (); ++i)
    {
      if (i->first == name)
        {
          _params->erase (i);
          return true;
        }
    }

  return false;
}

GINGA_NCL_END
