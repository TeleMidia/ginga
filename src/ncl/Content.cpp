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
#include "Content.h"

GINGA_NCL_BEGIN

Content::Content (const string &src)
{
  string type, extension;
  string::size_type index, len;

  type = "";
  if (src == "")
    goto done;

  index = src.find_last_of (".");
  if (index != std::string::npos)
    {
      index++;
      len = src.length ();
      if (index < len)
        {
          extension = src.substr (index, (len - index));
          if (extension != "")
            ginga_mime_table_index (extension, &type);
        }
    }

 done:
  _src = src;
  _type = type;
}

Content::~Content ()
{
}

string
Content::getSrc ()
{
  return _src;
}

string
Content::getType ()
{
  return _type;
}

GINGA_NCL_END
