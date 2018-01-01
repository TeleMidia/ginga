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

#include "ginga.h"
#include "ContentAvailabilityDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

ContentAvailabilityDescriptor::ContentAvailabilityDescriptor ()
{
  descriptorTag = 0xDE;
  descriptorLength = 0;
  retentionMode = 0;
  retentionState = 0;
  imageConstraintToken = 0;
  encriptionMode = 0;
}

ContentAvailabilityDescriptor::~ContentAvailabilityDescriptor () {}
unsigned char
ContentAvailabilityDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}
unsigned int
ContentAvailabilityDescriptor::getDescriptorLength ()
{
  return (unsigned int)descriptorLength;
}
void
ContentAvailabilityDescriptor::print ()
{
  clog << "ContentAvailabilityDescriptor::print printing..." << endl;
}
size_t
ContentAvailabilityDescriptor::process (char *data, size_t pos)
{
  // clog << "ContentAvailabiltyDescriptor::process with pos = " << pos;

  descriptorLength = data[pos + 1];
  // clog << " and length = " << (unsigned int)descriptorLength << endl;
  pos += 2;

  // 2 bits reserved for future use
  // clog << "CA debug = " << (data[pos] & 0xFF) << endl;
  imageConstraintToken = (unsigned char)((data[pos] & 0x20) >> 5); // 1 bit
  // clog << " CA ImageConstraingToken = " << (int)imageConstraintToken <<
  // endl;
  retentionMode = (unsigned char)((data[pos] & 0x10) >> 4); // 1 bit
  // clog << "CA retentionMode = " << (int)retentionMode << endl;
  retentionState = (unsigned char)((data[pos] & 0x0E) >> 1); // 3 bits
  // clog << "CA retentionState = " << (int)retentionState << endl;
  encriptionMode = (data[pos] & 0x01); // 1 bit
  // clog << "CA encriptionMode = " << (int)encriptionMode << endl;
  pos++;

  pos += descriptorLength - 2; // jumping reserved future use
  return pos;
}

GINGA_TSPARSER_END
