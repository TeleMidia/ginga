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
#include "NclMedia.h"

GINGA_NAMESPACE_BEGIN


// Public.

NclMedia::NclMedia (NclDocument *ncl, const string &id, bool settings)
  : NclNode (ncl, id)
{
  _isSettings = settings;
  _src = "";
  _mimetype = (settings) ? "application/x-ginga-settings" : "";
}

NclMedia::~NclMedia ()
{
  _instances.clear ();
}

bool
NclMedia::isSettings ()
{
  return _isSettings;
}

string
NclMedia::getMimeType ()
{
  return _mimetype;
}

void
NclMedia::setMimeType (const string &mimetype)
{
  _mimetype = mimetype;
}

string
NclMedia::getSrc ()
{
  return _src;
}

void
NclMedia::setSrc (const string &src)
{
 //  string type, extension;
 //  string::size_type index, len;

 //  type = "";
 //  if (src == "")
 //    goto done;

 //  index = src.find_last_of (".");
 //  if (index != std::string::npos)
 //    {
 //      index++;
 //      len = src.length ();
 //      if (index < len)
 //        {
 //          extension = src.substr (index, (len - index));
 //          if (extension != "")
 //            mime_table_index (extension, &type);
 //        }
 //    }

 // done:
  _src = src;
  // _mimetype = type;
}

void
NclMedia::addSameInstance (NclMediaRefer *node)
{
  g_assert_nonnull (node);
  _instances.insert (node);
}

const set<NclMediaRefer *> *
NclMedia::getInstSameInstances ()
{
  return &_instances;
}

GINGA_NAMESPACE_END
