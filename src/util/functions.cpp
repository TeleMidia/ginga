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
  vector<string> *params;

  params = split (utcValue, ":");
  switch (params->size ())
    {
    case 1:
      if (utcValue.find ("s") != std::string::npos)
        {
          utcValue = utcValue.substr (0, utcValue.length () - 1);
        }

      secs = xstrtod (utcValue);
      break;

    case 2:
      secs = 60 * xstrtod ((*params)[0])
             + xstrtod ((*params)[1]);
      break;

    case 3:
      secs = 3600 * xstrtod ((*params)[0])
             + 60 * xstrtod ((*params)[1])
             + xstrtod ((*params)[2]);
      break;

    default:
      secs = (double)INFINITY;
    }

  delete params;
  params = NULL;

  return secs;
}


static vector<string> *
localSplit (const string &str, const string &delimiter)
{
  vector<string> *splited;
  string::size_type lastPos, curPos;

  splited = new vector<string>;

  if (str == "")
    {
      return splited;
    }

  if (str.find_first_of (delimiter) == std::string::npos)
    {
      splited->push_back (str);
      return splited;
    }

  lastPos = str.find_first_not_of (delimiter, 0);

  if (lastPos != string::npos)
    {
      curPos = str.find_first_of (delimiter, lastPos);
      while (string::npos != curPos)
        {
          splited->push_back (str.substr (lastPos, curPos - lastPos));
          lastPos = str.find_first_not_of (delimiter, curPos);
          if (lastPos == string::npos)
            {
              break;
            }
          curPos = str.find_first_of (delimiter, lastPos);
          if (curPos == string::npos)
            {
              splited->push_back (
                  str.substr (lastPos, str.length () - lastPos));
            }
        }
    }
  return splited;
}

vector<string> *
split (const string &str, const string &delimiter, const string &pos_delimiter)
{
  vector<string> *splited;
  splited = new vector<string>;
  string::size_type pos = 0;
  string::size_type lastPos = 0;

  if (pos_delimiter == "")
    {
      return localSplit (str, delimiter);
    }

  while (string::npos != lastPos)
    {
      if (str.find_first_of (pos_delimiter, pos) == pos)
        {
          lastPos = str.find_last_of (pos_delimiter);
          if (string::npos != lastPos)
            {
              splited->push_back (str.substr (pos + 1, lastPos - pos - 1));
              lastPos = str.find_first_of (delimiter, lastPos);
              if (string::npos == lastPos)
                pos = lastPos;
              else
                pos = lastPos + 1;
            }
          else
            {
              return splited;
            }
        }
      else
        {
          lastPos = str.find_first_of (delimiter, pos);
          if (string::npos != lastPos)
            {
              splited->push_back (str.substr (pos, lastPos - pos));
              pos = lastPos + 1;
            }
        }
    }

  if (string::npos != pos)
    splited->push_back (str.substr (pos));

  return splited;
}

GINGA_UTIL_END
