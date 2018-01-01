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

#ifndef SERVICEDOMAIN_H_
#define SERVICEDOMAIN_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "DsmccDownloadServerInitiate.h"
#include "DsmccDownloadInfoIndication.h"
#include "DsmccDownloadDataBlock.h"
#include "DsmccObjectProcessor.h"
#include "DsmccModule.h"
#include "DsmccBiop.h"
#include "IDsmccServiceDomainListener.h"

GINGA_DATAPROC_BEGIN

class DsmccServiceDomain : public Thread
{
private:
  bool mountingServiceDomain;
  bool hasServiceGateway;
  DsmccIor *serviceGatewayIor;

  unsigned int carouselId;

  // mapping moduleId in module
  map<unsigned int, DsmccModule *> info;
  unsigned short blockSize;

  DsmccObjectProcessor *processor;
  string mountPoint;
  IDsmccServiceDomainListener *sdl;
  bool mounted;

  pthread_mutex_t stlMutex;

public:
  DsmccServiceDomain (DsmccDownloadServerInitiate *dsi,
                      DsmccDownloadInfoIndication *dii, unsigned short pid);

  virtual ~DsmccServiceDomain ();

  void setServiceDomainListener (IDsmccServiceDomainListener *sdl);
  void setObjectsListeners (set<IDsmccObjectListener *> *l);
  int receiveDDB (DsmccDownloadDataBlock *ddb);
  DsmccModule *getModuleById (unsigned int id);
  map<unsigned int, DsmccModule *> *getInfo ();
  unsigned short getBlockSize ();

  bool isMounted ();

private:
  DsmccModule *getModule (int position);
  void eraseModule (DsmccModule *module);
  bool hasModules ();

protected:
  virtual void run ();
};

GINGA_DATAPROC_END

#endif /*SERVICEDOMAIN_H_*/
