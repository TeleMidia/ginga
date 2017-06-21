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
#include "Entity.h"

GINGA_NCL_BEGIN

set<Entity *> Entity::_instances;

Entity::Entity (const string &id)
{
  this->_id = id;
  _typeSet.insert ("Entity");
  _instances.insert (this);
}

Entity::~Entity ()
{
  set<Entity *>::iterator i;

  i = _instances.find (this);
  if (i != _instances.end ())
    _instances.erase (i);
}

bool
Entity::hasInstance (Entity *instance, bool eraseFromList)
{
  set<Entity *>::iterator i;
  bool hasEntity = false;

  i = _instances.find (instance);
  if (i != _instances.end ())
    {
      if (eraseFromList)
        {
          _instances.erase (i);
        }
      hasEntity = true;
    }

  return hasEntity;
}

bool
Entity::instanceOf (const string &s)
{
  if (!_typeSet.empty ())
    {
      return (_typeSet.find (s) != _typeSet.end ());
    }
  else
    {
      return false;
    }
}

string
Entity::getId ()
{
  return _id;
}

void
Entity::setId (const string &someId)
{
  _id = someId;
}

Entity *
Entity::getDataEntity ()
{
  return this;
}

GINGA_NCL_END
