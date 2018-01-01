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
#include "LocalTimeOffsetDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

LocalTimeOffsetDescriptor::LocalTimeOffsetDescriptor ()
{
  descriptorTag = 0x58;
  descriptorLength = 0;
  countryRegionId = 0;
  localTimeOffset = 0;
  localTimeOffsetPolarity = 0;
  nextTimeOffset = 0;
  memset (countryCode, 0, 3);
  memset (timeOfChange, 0, 5);
}

LocalTimeOffsetDescriptor::~LocalTimeOffsetDescriptor () {}

unsigned char
LocalTimeOffsetDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
LocalTimeOffsetDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}

string
LocalTimeOffsetDescriptor::getCountryCode ()
{
  string str;
  str.assign (countryCode, 3);

  return str;
}

unsigned char
LocalTimeOffsetDescriptor::getCountryRegionId ()
{
  return countryRegionId;
}

unsigned char
LocalTimeOffsetDescriptor::getLocalTimeOffsetPolarity ()
{
  return localTimeOffsetPolarity;
}

unsigned short
LocalTimeOffsetDescriptor::getLocalTimeOffset ()
{
  return localTimeOffset;
}

string
LocalTimeOffsetDescriptor::getTimeOfChange ()
{
  string str;
  str.assign (timeOfChange, 5);

  return str;
}

unsigned short
LocalTimeOffsetDescriptor::getNextTimeOffset ()
{
  return nextTimeOffset;
}
void
LocalTimeOffsetDescriptor::print ()
{
  clog << "LocalTimeOffsetDescriptor::print printing..." << endl;
  clog << "CountryCode: " << getCountryCode () << endl;
  clog << "CountryRegionId: " << (countryRegionId & 0xFF) << endl;
  clog << "LocalTimeOffsetPolarity: " << (localTimeOffsetPolarity & 0xFF);
  clog << endl;
  clog << "LocalTimeOffSet:" << (localTimeOffset & 0xFF) << endl;
  clog << "TimeOfChange: " << getTimeOfChange () << endl;
  clog << "NextTimeOffset: " << getNextTimeOffset () << endl;
}

size_t
LocalTimeOffsetDescriptor::process (char *data, size_t pos)
{
  clog << " LocalTimeOffsetDescriptor::process" << endl;
  descriptorLength = data[pos + 1];
  pos += 2;

  memcpy (countryCode, data + pos, 3);
  pos += 3;

  countryRegionId = ((data[pos] & 0xFC) >> 2);
  localTimeOffsetPolarity = data[pos] & 0x01;
  pos++;

  localTimeOffset
      = ((((data[pos] & 0xFF) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));
  pos += 2;

  memcpy (timeOfChange, data + pos, 5);
  pos += 5;

  nextTimeOffset
      = ((((data[pos] & 0xFF) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));

  pos++;

  return pos;
}

GINGA_TSPARSER_END
