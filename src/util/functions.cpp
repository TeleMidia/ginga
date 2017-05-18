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
#include "functions.h"

extern "C" {
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
}

GINGA_UTIL_BEGIN

string userCurrentPath;

double
strUTCToSec (const string &utcV)
{
  string utcValue = utcV;
  double secs = 0;
  vector<string> params;

  params = split (utcValue, ':');
  switch (params.size ())
    {
    case 1:
      if (utcValue.find ("s") != std::string::npos)
        {
          utcValue = utcValue.substr (0, utcValue.length () - 1);
        }

      secs = xstrtod (utcValue);
      break;

    case 2:
      secs = 60 * xstrtod (params[0])
             + xstrtod (params[1]);
      break;

    case 3:
      secs = 3600 * xstrtod (params[0])
             + 60 * xstrtod (params[1])
             + xstrtod (params[2]);
      break;

    default:
      secs = (double)INFINITY;
    }

  return secs;
}

vector<string>
split(const string &s, char delim)
{
  vector<string> internal;
  stringstream ss (s);
  string tok;

  while(getline (ss, tok, delim))
    {
      internal.push_back(tok);
    }

  return internal;
}

GINGA_UTIL_END
