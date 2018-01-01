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

#ifndef MESSAGEPROCESSOR_H_
#define MESSAGEPROCESSOR_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "isdbt-tsparser/ITransportSection.h"
using namespace ::ginga::tsparser;

#include "DsmccServiceDomain.h"
#include "DsmccMessageHeader.h"
#include "DsmccDownloadServerInitiate.h"
#include "DsmccDownloadInfoIndication.h"
#include "DsmccDownloadDataBlock.h"

GINGA_DATAPROC_BEGIN

class DsmccMessageProcessor
{
private:
  DsmccServiceDomain *sd;
  DsmccDownloadServerInitiate *dsi;
  DsmccDownloadInfoIndication *dii;
  vector<DsmccMessageHeader *> msgs;
  pthread_mutex_t msgMutex;
  unsigned short pid;

public:
  DsmccMessageProcessor (unsigned short pid);
  virtual ~DsmccMessageProcessor ();

  DsmccServiceDomain *pushMessage (DsmccMessageHeader *hdr);

private:
  DsmccServiceDomain *processDSIMessage (DsmccMessageHeader *message);
  DsmccServiceDomain *processDIIMessage (DsmccMessageHeader *message);
  void processDDBMessages ();

public:
  DsmccServiceDomain *getServiceDomain ();
  void checkTasks ();
};

GINGA_DATAPROC_END

#endif /*MESSAGEPROCESSOR_H_*/
