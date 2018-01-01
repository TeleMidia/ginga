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

#ifndef AIT_H_
#define AIT_H_

#include "IMpegDescriptor.h"

#include "Application.h"
#include "ApplicationDescriptor.h"
#include "ApplicationLocationDescriptor.h"
#include "ApplicationNameDescriptor.h"
#include "PrefetchDescriptor.h"
#include "TransportProtocolDescriptor.h"

#include "IAIT.h"

GINGA_TSPARSER_BEGIN

class AIT : public IAIT
{
private:
  unsigned int applicationType;
  unsigned short commonDescriptorsLength;
  vector<IMpegDescriptor *> descriptors;
  unsigned short applicationLoopLength;
  vector<IApplication *> applications;
  string secName;

public:
  AIT ();
  ~AIT ();
  string getSectionName ();
  void setSectionName (const string &secName);
  void setApplicationType (unsigned int type);
  void process (void *payloadBytes, unsigned int payloadSize);
  vector<IMpegDescriptor *> *copyDescriptors ();
  vector<IApplication *> *copyApplications ();
};

GINGA_TSPARSER_END

#endif /* AIT_H_ */
