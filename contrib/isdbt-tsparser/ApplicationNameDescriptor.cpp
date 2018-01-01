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

#include "ginga.h"
#include "ApplicationNameDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

ApplicationNameDescriptor::ApplicationNameDescriptor ()
{
  descriptorLength = 0;
  descriptorTag = 0x01;
}

ApplicationNameDescriptor::~ApplicationNameDescriptor ()
{
  vector<struct AppName *>::iterator i;

  i = appNames.begin ();
  while (i != appNames.end ())
    {
      delete (*i)->applicationNameChar;
      delete (*i);
      ++i;
    }
  appNames.clear ();
}

unsigned char
ApplicationNameDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
ApplicationNameDescriptor::getDescriptorLength ()
{
  return descriptorLength;
}

void
ApplicationNameDescriptor::print ()
{
}

size_t
ApplicationNameDescriptor::process (char *data, size_t pos)
{
  unsigned char remainingBytes;
  struct AppName *appName;

  descriptorTag = data[pos];
  descriptorLength = data[pos + 1];
  pos++;

  remainingBytes = descriptorLength;
  while (remainingBytes > 0)
    {
      appName = new struct AppName;
      pos++;

      memcpy (appName->languageCode, data + pos, 3);
      pos += 3;

      appName->applicationNameLength = data[pos];
      appName->applicationNameChar
          = new char[appName->applicationNameLength];

      memcpy (appName->applicationNameChar, data + pos + 1,
              appName->applicationNameLength);

      pos += appName->applicationNameLength;
      remainingBytes -= (appName->applicationNameLength + 4);
      appNames.push_back (appName);
    }

  return pos;
}

GINGA_TSPARSER_END
