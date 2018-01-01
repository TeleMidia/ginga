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

#ifndef STREAMEVENT_H_
#define STREAMEVENT_H_

#include "ginga.h"

GINGA_DATAPROC_BEGIN

class DsmccStreamEvent
{
private:
  unsigned int descriptorTag;
  unsigned int descriptorLength;
  unsigned int eventId;
  uint64_t timeReference;
  string eventName;
  char data[262]; // 7 header bytes + 255 data field bytes

public:
  DsmccStreamEvent (void *descriptorData, unsigned int descriptorSize);
  virtual ~DsmccStreamEvent ();
  unsigned int getDescriptorTag ();
  unsigned int getDescriptorLength ();
  unsigned int getId ();
  long double getTimeReference ();
  char *getData ();
  void *getEventData ();
  void setEventName (const string &eventName);
  string getEventName ();
  void print ();
};

GINGA_DATAPROC_END

#endif /* STREAM_EVENT_H */
