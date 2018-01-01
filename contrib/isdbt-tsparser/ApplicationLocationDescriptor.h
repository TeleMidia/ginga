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

#ifndef APPLICATION_LOCATION_DESCRIPTOR_H
#define APPLICATION_LOCATION_DESCRIPTOR_H

#include "ginga.h"

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class ApplicationLocationDescriptor : public IMpegDescriptor
{
private:
  unsigned char baseDirectoryLength;
  char *baseDirectoryByte;
  unsigned char classPathExtensionLength;
  char *classPathExtensionByte;
  unsigned char initialClassLentgh;
  char *initialClassByte;

public:
  ApplicationLocationDescriptor ();
  virtual ~ApplicationLocationDescriptor ();
  unsigned char getDescriptorTag ();
  unsigned int getDescriptorLength ();
  size_t process (char *data, size_t pos);
  void print ();
  unsigned int getBaseDirectoryLength ();
  string getBaseDirectory ();
  unsigned int getClassPathExtensionLength ();
  string getClassPathExtension ();
  unsigned int getInitialClassLength ();
  string getInitialClass ();
};

GINGA_TSPARSER_END

#endif /* APPLICATION_LOCATION_DESCRIPTOR_H */
