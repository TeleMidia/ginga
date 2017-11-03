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

#include "NclEntity.h"
#include "NclDocument.h"
#include "NclProperty.h"

GINGA_NAMESPACE_BEGIN

NclEntity::NclEntity (NclDocument *ncl, const string &id)
{
  g_assert (id != "");
  _id = id;
  _ncl = ncl;

  if (!instanceof (NclProperty *, this))
    ncl->registerEntity (this); // fixme: assert
}

NclEntity::~NclEntity ()
{
}

string
NclEntity::getId ()
{
  return _id;
}

NclDocument *
NclEntity::getDocument ()
{
  return _ncl;
}

// void *
// NclEntity::getData (const string &key)
// {
//   map<string, void *>::iterator it;
//   return ((it = _userdata.find (key)) == _userdata.end ())
//     ? nullptr : it->second;
// }

// void
// NclEntity::setData (const string &key, void *data)
// {
//   _userdata[key] = data;
// }

GINGA_NAMESPACE_END
