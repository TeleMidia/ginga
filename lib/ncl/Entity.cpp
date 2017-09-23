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

#include "Entity.h"
#include "NclDocument.h"
#include "Property.h"

GINGA_NCL_BEGIN

/**
 * @brief Creates a new entity.
 * @param ncl Parent document.
 * @param id Entity id.
 */
Entity::Entity (NclDocument *ncl, const string &id)
{
  g_assert (id != "");
  _id = id;
  _ncl = ncl;

  if (!instanceof (Property *, this))
    ncl->registerEntity (this); // fixme: assert
}

/**
 * @brief Destroys entity.
 */
Entity::~Entity ()
{
}

/**
 * @brief Gets entity id.
 */
string
Entity::getId ()
{
  return _id;
}

/**
 * @brief Gets entity document.
 */
NclDocument *
Entity::getDocument ()
{
  return _ncl;
}

/**
 * @brief Gets data previously attached to entity.
 * @param key Name of the key.
 * @return Data associated with key.
 */
void *
Entity::getData (const string &key)
{
  map<string, void *>::iterator it;
  return ((it = _userdata.find (key)) == _userdata.end ())
    ? nullptr : it->second;
}

/**
 * @brief Attaches data to entity.
 * @param key Name of the key.
 * @param data Data to associate with key.
 */
void
Entity::setData (const string &key, void *data)
{
  _userdata[key] = data;
}

GINGA_NCL_END
