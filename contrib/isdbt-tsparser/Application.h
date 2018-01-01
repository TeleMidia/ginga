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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "IMpegDescriptor.h"

#include "ApplicationDescriptor.h"
#include "ApplicationLocationDescriptor.h"
#include "ApplicationNameDescriptor.h"
#include "PrefetchDescriptor.h"
#include "TransportProtocolDescriptor.h"

#include "IApplication.h"

#include "system/Thread.h"
using namespace ::ginga::system;

GINGA_TSPARSER_BEGIN

typedef struct
{
  unsigned int organizationId;
  unsigned short applicationId;
} ApplicationIdentifier;

class Application : public IApplication
{
private:
  ApplicationIdentifier applicationId;
  unsigned short applicationControlCode;
  unsigned short appDescriptorsLoopLength;
  vector<IMpegDescriptor *> descriptors;
  unsigned short applicationLength;
  pthread_mutex_t stlMutex;

public:
  Application ();
  virtual ~Application ();

  string getBaseDirectory ();
  string getInitialClass ();
  string getId ();
  unsigned short getControlCode ();
  unsigned short getLength ();
  unsigned short getProfile (int profileNumber = 0);
  unsigned short getTransportProtocolId ();
  size_t process (char *data, size_t pos);
};

GINGA_TSPARSER_END

#endif /* APPLICATION_H_ */
