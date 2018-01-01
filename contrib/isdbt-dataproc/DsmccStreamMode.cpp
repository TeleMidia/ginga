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
#include "DsmccStreamMode.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)

GINGA_DATAPROC_BEGIN

DsmccStreamMode::DsmccStreamMode () : DsmccMpegDescriptor (0x03)
{
  streamMode = 0;
  descriptorLength = 2;
}

DsmccStreamMode::~DsmccStreamMode () {}

int
DsmccStreamMode::process ()
{
  int pos = DsmccMpegDescriptor::process ();
  streamMode = stream[pos++] & 0xFF;
  return 0;
}

int
DsmccStreamMode::updateStream ()
{
  int pos = DsmccMpegDescriptor::updateStream ();

  stream[pos++] = streamMode & 0xFF;
  stream[pos++] = 0xFF;

  return pos;
}

unsigned int
DsmccStreamMode::calculateDescriptorSize ()
{
  int pos = DsmccMpegDescriptor::calculateDescriptorSize ();
  return pos + 2;
}

unsigned char
DsmccStreamMode::getStreamMode ()
{
  return streamMode;
}

void
DsmccStreamMode::setStreamMode (unsigned char mode)
{
  streamMode = mode;
}

GINGA_DATAPROC_END
