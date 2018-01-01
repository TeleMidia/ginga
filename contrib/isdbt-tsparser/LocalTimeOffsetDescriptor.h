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

#ifndef LOCALTIMEOFFSETDESCRIPTOR_H_
#define LOCALTIMEOFFSETDESCRIPTOR_H_

#include "IMpegDescriptor.h"

GINGA_TSPARSER_BEGIN

class LocalTimeOffsetDescriptor : public IMpegDescriptor
{
protected:
  char countryCode[3];
  unsigned char countryRegionId;
  unsigned char localTimeOffsetPolarity;
  unsigned short localTimeOffset;
  char timeOfChange[5];
  unsigned short nextTimeOffset;

public:
  LocalTimeOffsetDescriptor ();
  ~LocalTimeOffsetDescriptor ();
  unsigned char getDescriptorTag ();
  unsigned int getDescriptorLength ();
  string getCountryCode ();
  unsigned char getCountryRegionId ();
  unsigned char getLocalTimeOffsetPolarity ();
  unsigned short getLocalTimeOffset ();
  string getTimeOfChange ();
  unsigned short getNextTimeOffset ();
  void print ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /* LOCALTIMEOFFSETDESCRIPTOR_H_ */
