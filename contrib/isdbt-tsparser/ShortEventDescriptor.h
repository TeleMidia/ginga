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

#ifndef SHORTEVENTDESCRIPTOR_H_
#define SHORTEVENTDESCRIPTOR_H_

#include "IMpegDescriptor.h"

#include "IShortEventDescriptor.h"

GINGA_TSPARSER_BEGIN

class ShortEventDescriptor : public IShortEventDescriptor
{
protected:
  char languageCode[3];
  unsigned char eventNameLength;
  char *eventNameChar;
  unsigned char textLength;
  char *textChar;

public:
  ShortEventDescriptor ();
  ~ShortEventDescriptor ();
  unsigned char getDescriptorTag ();
  unsigned int getDescriptorLength ();
  string getLanguageCode ();
  unsigned int getEventNameLength ();
  string getEventName ();
  unsigned int getTextLength ();
  string getTextChar ();
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /*SHORTEVENTDESCRIPTOR_H_*/
