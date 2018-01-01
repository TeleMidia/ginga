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

#ifndef _EventDescriptor_H_
#define _EventDescriptor_H_

GINGA_DATAPROC_BEGIN

class NCLEventDescriptor
{
public:
  static string getEventId (const string &event);
  static uint64_t getEventNPT (const string &event);
  static string getCommandTag (const string &event);
  static int getSequenceNumber (const string &event);
  static bool getFinalFlag (const string &event);
  static string getPrivateDataPayload (const string &event);
  static bool checkFCS (const string &event);

  static string extractMarks (const string &eventParam);
};

GINGA_DATAPROC_END

#endif //_EventDescriptor_H_
