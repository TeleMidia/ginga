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
#include "util/functions.h"
using namespace ::ginga::util;

#include "NCLEventDescriptor.h"

GINGA_DATAPROC_BEGIN

string
NCLEventDescriptor::getEventId (const string &event)
{
  return event.substr (0, 2);
}

uint64_t
NCLEventDescriptor::getEventNPT (const string &event)
{
  uint64_t nptRef = 0;
  char *strNpt;

  strNpt = deconst (char *, event.substr (5, 5).c_str ());

  nptRef = (strNpt[0] & 0x01);
  nptRef = nptRef << 8;
  nptRef = nptRef | (strNpt[1] & 0xFF);
  nptRef = nptRef << 8;
  nptRef = nptRef | (strNpt[2] & 0xFF);
  nptRef = nptRef << 8;
  nptRef = nptRef | (strNpt[3] & 0xFF);
  nptRef = nptRef << 8;
  nptRef = nptRef | (strNpt[4] & 0xFF);

  return nptRef;
}

string
NCLEventDescriptor::getCommandTag (const string &event)
{
  string cmdTag = "0x" + xstrbuild ("%d", event[11] & 0xFF);

  return cmdTag;
}

int
NCLEventDescriptor::getSequenceNumber (const string &event)
{
  char *strSeq;

  strSeq = deconst (char *, event.substr (12, 1).c_str ());
  return strSeq[0] & 0xFE;
}

bool
NCLEventDescriptor::getFinalFlag (const string &event)
{
  char *strFF;

  strFF = deconst (char *, event.substr (12, 1).c_str ());
  return strFF[0] & 0x01;
}

string
NCLEventDescriptor::getPrivateDataPayload (const string &event)
{
  unsigned int privateDataLength;

  privateDataLength = event[10] & 0xFF;
  if (privateDataLength + 11 != event.length ())
    {
      clog << "NCLEventDescriptor::getPrivateDataPayload Warning! ";
      clog << "invalid private data length(" << privateDataLength;
      clog << ") for event length(" << event.length () << ")";
      clog << endl;
    }
  return event.substr (13, privateDataLength - 3);
}

bool
NCLEventDescriptor::checkFCS (unused (const string &event))
{
  return true;
}

string
NCLEventDescriptor::extractMarks (const string &eventParam)
{
  string noMarks = xstrchomp (eventParam);

  if (eventParam.find ("\"") != std::string::npos)
    {
      noMarks = eventParam.substr (
          eventParam.find_first_of ("\"") + 1,
          eventParam.length () - (eventParam.find_first_of ("\"") + 1));

      noMarks = noMarks.substr (0, noMarks.find_last_of ("\""));
    }

  return noMarks;
}

GINGA_DATAPROC_END
