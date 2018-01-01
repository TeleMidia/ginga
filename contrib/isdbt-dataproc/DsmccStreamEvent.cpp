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
#include "DsmccStreamEvent.h"

GINGA_PRAGMA_DIAG_IGNORE (-Wsign-conversion)

GINGA_DATAPROC_BEGIN

DsmccStreamEvent::DsmccStreamEvent (void *descriptorData,
                                    unsigned int descriptorSize)
{
  memset (data, 0, sizeof (data));
  memcpy ((void *)&(data[0]), descriptorData, descriptorSize);

  this->descriptorTag = (data[0] & 0xFF);
  this->descriptorLength = (data[1] & 0xFF);
  this->eventId = ((data[2] & 0xFF) << 8) | (data[3] & 0xFF);

  // 4,5,6,7* reserved
  this->timeReference = (((uint64_t) (data[7] & 0x01)) << 32)
                        | ((data[8] & 0xFF) << 24)
                        | ((data[9] & 0xFF) << 16)
                        | ((data[10] & 0xFF) << 8) | (data[11] & 0xFF);
}

DsmccStreamEvent::~DsmccStreamEvent () {}

unsigned int
DsmccStreamEvent::getDescriptorTag ()
{
  return descriptorTag;
}

unsigned int
DsmccStreamEvent::getDescriptorLength ()
{
  return descriptorLength;
}

unsigned int
DsmccStreamEvent::getId ()
{
  return eventId;
}

long double
DsmccStreamEvent::getTimeReference ()
{
  return timeReference;
}

char *
DsmccStreamEvent::getData ()
{
  return data + 2;
}

void *
DsmccStreamEvent::getEventData ()
{
  return (void *)&(data[12]);
}

void
DsmccStreamEvent::setEventName (const string &name)
{
  this->eventName = name;
}

string
DsmccStreamEvent::getEventName ()
{
  return eventName;
}

void
DsmccStreamEvent::print ()
{
  clog << "descriptorTag: " << descriptorTag << endl;
  clog << "descriptorLength: " << descriptorLength << endl;
  clog << "eventId: " << eventId << endl;
  clog << "eventNPT: " << timeReference << endl;
  clog << "eventName: " << eventName << endl;
  clog << "privateData: " << (string) (char *)getEventData () << endl;
}

GINGA_DATAPROC_END
