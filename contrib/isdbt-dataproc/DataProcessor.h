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

#ifndef DATAPROCESSOR_H_
#define DATAPROCESSOR_H_

#include "DsmccMessageProcessor.h"
#include "DsmccNPTProcessor.h"
#include "DsmccServiceDomain.h"
#include "EPGProcessor.h"
#include "FilterManager.h"
#include "IDsmccObjectListener.h"
#include "IDsmccObjectListener.h"
#include "IDsmccServiceDomainListener.h"
#include "IDsmccServiceDomainListener.h"
#include "IDsmccStreamEventListener.h"
#include "IDsmccStreamEventListener.h"
#include "IEPGListener.h"
#include "IEPGListener.h"

#include "system/ITimeBaseProvider.h"
using namespace ::ginga::system;

#include "isdbt-tuner/ISTCProvider.h"
using namespace ::ginga::tuner;

#include "isdbt-tsparser/IAIT.h"
#include "isdbt-tsparser/IDemuxer.h"
#include "isdbt-tsparser/IFilterListener.h"
#include "isdbt-tsparser/IMpegDescriptor.h"
using namespace ::ginga::tsparser;

GINGA_DATAPROC_BEGIN

struct notifyData
{
  IDsmccStreamEventListener *listener;
  DsmccStreamEvent *se;
};

class DataProcessor : public IFilterListener,
                      public ITunerListener,
                      public IDsmccServiceDomainListener,
                      public Thread
{
private:
  EPGProcessor *epgProcessor;
  FilterManager *filterManager;
  map<unsigned int, DsmccMessageProcessor *> processors;
  map<string, set<IDsmccStreamEventListener *> *> eventListeners;
  set<IDsmccObjectListener *> objectListeners;
  IDsmccServiceDomainListener *sdl;
  set<unsigned int> processedIds;
  DsmccNPTProcessor *nptProcessor;
  vector<ITransportSection *> sections;
  IDemuxer *demux;
  IAIT *ait;
  bool running;
  bool removeOCFilter;
  bool nptPrinter;

public:
  DataProcessor ();

private:
  virtual ~DataProcessor ();

public:
  void deleteAIT ();
  void setNptPrinter (bool nptPrinter);

  bool applicationInfoMounted (IAIT *ait);
  void serviceDomainMounted (const string &mountPoint, const map<string, string> *names,
                             const map<string, string> *paths);

  void setDemuxer (IDemuxer *demux);
  void removeOCFilterAfterMount (bool removeIt);

  void setSTCProvider (ISTCProvider *stcProvider);
  ITimeBaseProvider *getNPTProvider ();

  void createStreamTypeSectionFilter (short streamType);
  void createPidSectionFilter (int pid);

  void addSEListener (const string &eventType,
                      IDsmccStreamEventListener *listener);

  void removeSEListener (const string &eventType,
                         IDsmccStreamEventListener *listener);

  void setServiceDomainListener (IDsmccServiceDomainListener *listener);
  void addObjectListener (IDsmccObjectListener *listener);
  void removeObjectListener (IDsmccObjectListener *listener);

private:
  void notifySEListeners (DsmccStreamEvent *se);
  static void *notifySEListener (void *data);
  void notifyEitListeners (set<IEventInfo *> *events);

public:
  void receiveData (unused (char *buff),
                    unused (unsigned int size)){};
  void receiveSection (ITransportSection *section);
  void updateChannelStatus (short newStatus, Channel *channel);
  bool isReady ();

private:
  void run ();
};

GINGA_DATAPROC_END

#endif /*DataProcessor_H_*/
