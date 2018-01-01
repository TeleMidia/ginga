/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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

#ifndef DATACONTENTDESCRIPTOR_H_
#define DATACONTENTDESCRIPTOR_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class DataContentDescriptor : public IMpegDescriptor
{
protected:
  unsigned short dataComponentId;
  unsigned char entryComponent;
  unsigned char selectorLength;
  char *selectorByte;
  unsigned char numOfComponentRef;
  char *componentRef;
  char languageCode[3]; // epg metadata
  unsigned char textLength;
  char *textChar; // epg metadata

public:
  DataContentDescriptor ();
  ~DataContentDescriptor ();
  unsigned char getDescriptorTag ();
  unsigned int getDescriptorLength ();
  string getLanguageCode ();
  string getTextChar ();
  unsigned int getTextLength ();
  unsigned int getSelectorLength ();
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /* DATACONTENTDESCRIPTOR_H_ */
