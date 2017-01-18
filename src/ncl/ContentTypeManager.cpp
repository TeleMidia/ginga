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

#include "config.h"
#include "ContentTypeManager.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_NCL_BEGIN

ContentTypeManager *ContentTypeManager::_instance = NULL;
string ContentTypeManager::absUrl = "";

ContentTypeManager::ContentTypeManager ()
{
  pthread_mutex_init (&mutex, NULL);
}

ContentTypeManager *
ContentTypeManager::getInstance ()
{
  if (_instance == NULL)
    {
      _instance = new ContentTypeManager ();
    }
  return _instance;
}

void
ContentTypeManager::setMimeFile (string mimeFile)
{
  if (mimeFile != absUrl)
    {
      absUrl = mimeFile;
      readMimeDefinitions ();
    }
}

void
ContentTypeManager::readMimeDefinitions ()
{
  ifstream fisMime;
  string line, key, value;

  fisMime.open (absUrl.c_str (), ifstream::in);

  if (!fisMime.is_open ())
    {
      clog << "ContentTypeAdapterManager::readMimeDefinitions ";
      clog << "Warning! Can't open input file: '" << absUrl;
      clog << "'" << endl;
      return;
    }

  while (fisMime.good ())
    {
      fisMime >> line;
      key = line.substr (0, line.find_last_of ("="));
      value = line.substr ((line.find_first_of ("=") + 1),
                           line.length () - (line.find_first_of ("=") + 1));

      mimeDefaultTable[key] = value;
    }

  fisMime.close ();
}

string
ContentTypeManager::getMimeType (string fileExtension)
{
  string mType = "";

  pthread_mutex_lock (&mutex);
  if (fileExtension != "" && mimeDefaultTable.count (fileExtension) != 0)
    {

      mType = mimeDefaultTable[fileExtension];
    }
  pthread_mutex_unlock (&mutex);

  return mType;
}

GINGA_NCL_END
