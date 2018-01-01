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
#include "ApplicationLocationDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

ApplicationLocationDescriptor::ApplicationLocationDescriptor ()
{
  baseDirectoryByte = NULL;
  baseDirectoryLength = 0;
  classPathExtensionByte = NULL;
  classPathExtensionLength = 0;
  descriptorTag = 0;
  initialClassByte = NULL;
  initialClassLentgh = 0;
}

ApplicationLocationDescriptor::~ApplicationLocationDescriptor ()
{
  if (baseDirectoryByte != NULL)
    {
      delete baseDirectoryByte;
    }
  if (classPathExtensionByte != NULL)
    {
      delete classPathExtensionByte;
    }
  if (initialClassByte != NULL)
    {
      delete initialClassByte;
    }
}

unsigned char
ApplicationLocationDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
ApplicationLocationDescriptor::getDescriptorLength ()
{
  return descriptorLength;
}

string
ApplicationLocationDescriptor::getBaseDirectory ()
{
  string baseDir;
  baseDir.assign (baseDirectoryByte, (baseDirectoryLength & 0xFF));

  return baseDir;
}

unsigned int
ApplicationLocationDescriptor::getBaseDirectoryLength ()
{
  return baseDirectoryLength;
}

unsigned int
ApplicationLocationDescriptor::getClassPathExtensionLength ()
{
  return classPathExtensionLength;
}

string
ApplicationLocationDescriptor::getClassPathExtension ()
{
  string classPath;
  classPath.assign (classPathExtensionByte,
                    (classPathExtensionLength & 0xFF));

  return classPath;
}

unsigned int
ApplicationLocationDescriptor::getInitialClassLength ()
{
  return initialClassLentgh;
}

string
ApplicationLocationDescriptor::getInitialClass ()
{
  string initialClass;
  initialClass.assign (initialClassByte, (initialClassLentgh & 0xFF));

  return initialClass;
}

void
ApplicationLocationDescriptor::print ()
{
}

size_t
ApplicationLocationDescriptor::process (char *data, size_t pos)
{
  descriptorTag = data[pos];
  descriptorLength = data[pos + 1];
  pos += 2;

  baseDirectoryLength = data[pos];

  baseDirectoryByte = new char[baseDirectoryLength];
  memcpy (baseDirectoryByte, data + pos + 1, baseDirectoryLength);
  pos += baseDirectoryLength + 1;

  classPathExtensionLength = data[pos];

  classPathExtensionByte = new char[classPathExtensionLength];
  memcpy (classPathExtensionByte, data + pos + 1, classPathExtensionLength);
  pos += classPathExtensionLength + 1;

  initialClassLentgh = descriptorLength - baseDirectoryLength
                       - classPathExtensionLength - 2;

  initialClassByte = new char[initialClassLentgh];
  memcpy (initialClassByte, data + pos, initialClassLentgh);

  pos += initialClassLentgh;

  string baseDir, iniClass;
  baseDir.assign (baseDirectoryByte, (baseDirectoryLength & 0xFF));
  iniClass.assign (initialClassByte, (initialClassLentgh & 0xFF));

  return pos;
}

GINGA_TSPARSER_END
