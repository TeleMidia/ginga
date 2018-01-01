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
#include "TransportProtocolDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_TSPARSER_BEGIN

TransportProtocolDescriptor::TransportProtocolDescriptor ()
{
  protocolId = 0;
  transportProtocolLabel = 0;
  selectorByte = NULL;
  selectorByteLength = 0;
  descriptorLength = 0;
  descriptorTag = 0x02;
}

TransportProtocolDescriptor::~TransportProtocolDescriptor ()
{
  if (selectorByte != NULL)
    {
      delete selectorByte;
    }
}

unsigned int
TransportProtocolDescriptor::getDescriptorLength ()
{
  return descriptorLength;
}

unsigned char
TransportProtocolDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
TransportProtocolDescriptor::getSelectorByteLength ()
{
  return selectorByteLength;
}

char *
TransportProtocolDescriptor::getSelectorByte ()
{
  return selectorByte;
}

unsigned char
TransportProtocolDescriptor::getTransportProtocolLabel ()
{
  return transportProtocolLabel;
}

unsigned short
TransportProtocolDescriptor::getProtocolId ()
{
  return protocolId;
}

void
TransportProtocolDescriptor::print ()
{
  clog << "TransportProtocolDescriptor::print" << endl;
}

size_t
TransportProtocolDescriptor::process (char *data, size_t pos)
{
  descriptorLength = data[pos + 1];
  pos += 2;

  protocolId
      = ((((data[pos] & 0xFF) << 8) & 0xFF00) | (data[pos + 1] & 0xFF));
  pos += 2;

  transportProtocolLabel = data[pos];
  // pos ++;

  selectorByteLength = descriptorLength - 3;
  selectorByte = new char[selectorByteLength];

  memcpy (selectorByte, data + pos + 1, selectorByteLength);

  pos += selectorByteLength;

  return pos;
}

GINGA_TSPARSER_END
