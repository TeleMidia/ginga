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
#include "StreamIdentifierDescriptor.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_TSPARSER_BEGIN

StreamIdentifierDescriptor::StreamIdentifierDescriptor ()
{
  descriptorTag = 0x52;
  descriptorLength = 0;
  componentTag = 0;
}

StreamIdentifierDescriptor::~StreamIdentifierDescriptor () {}

unsigned char
StreamIdentifierDescriptor::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
StreamIdentifierDescriptor::getDescriptorLength ()
{
  return descriptorLength;
}

unsigned char
StreamIdentifierDescriptor::getComponentTag ()
{
  return componentTag;
}

void
StreamIdentifierDescriptor::print ()
{
  clog << "StreamIdentifierDescriptor::print ctag = "
       << (componentTag & 0xFF) << endl;
}

size_t
StreamIdentifierDescriptor::process (char *data, size_t pos)
{
  descriptorLength = data[pos + 1];
  pos += 2;
  componentTag = data[pos];

  return pos;
}

GINGA_TSPARSER_END
