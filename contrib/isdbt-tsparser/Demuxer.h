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

#ifndef DEMUXER_H_
#define DEMUXER_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "Pmt.h"
#include "Pat.h"

#include "FrontendFilter.h"
#include "TSPacket.h"
#include "ITSFilter.h"
#include "IDemuxer.h"

GINGA_TSPARSER_BEGIN

class Demuxer : public IDemuxer
{
private:
  Pat *pat;
  Pmt *newPmt;
  map<unsigned int, Pmt *> pmts;
  map<unsigned int, ITSFilter *> pidFilters;
  map<short, ITSFilter *> stFilters;
  set<IFrontendFilter *> feFilters;
  set<IFrontendFilter *> feFiltersToSetup;
  static vector<Pat *> pats;
  static unsigned int sectionPid; // debug only
  static set<unsigned int> knownSectionPids;
  Tuner *tuner;

  short debugDest;
  unsigned int debugPacketCounter;

  pthread_mutex_t flagLockUntilSignal;
  pthread_cond_t flagCondSignal;

  static pthread_mutex_t stlMutex;

  unsigned char packetSize;
  bool nptPrinter;
  int nptPid;

  string outPipeUri;
  PipeDescriptor outPipeD;
  bool outPipeCreated;

  list<Buffer *> demuxMe;

public:
  // defs
  static const short NB_PID_MAX = 0x1FFF; // 8191
  static const short ERR_CONDITION_SATISFIED = 5;

public:
  Demuxer (Tuner *tuner);
  virtual ~Demuxer ();

  string createTSUri (const string &tsOutputUri);
  bool hasStreamType (short streamType);
  void printPat ();
  void setNptPrinter (bool nptPrinter);

private:
  void createPSI ();
  void clearPSI ();
  void initMaps ();
  void clearMaps ();
  void resetDemuxer ();
  void setDestination (short streamType); // debug purpose only
  void removeFilter (IFrontendFilter *filter);
  void setupUnsolvedFilters ();
  bool setupFilter (IFrontendFilter *filter);
  void demux (ITSPacket *packet);
  static void *createNullDemuxer (void *ptr);

public:
  map<unsigned int, Pmt *> *getProgramsInfo ();
  unsigned int getTSId ();
  Pat *getPat ();
  int getDefaultMainVideoPid ();
  int getDefaultMainAudioPid ();
  int getDefaultMainCarouselPid ();
  void removeFilter (ITSFilter *tsFilter);
  void addFilter (ITSFilter *tsFilter, int pid, int tid);
  void addFilter (IFrontendFilter *filter);

private:
  void attachFilter (IFrontendFilter *filter);
  void createPatFilter (NetworkInterface *ni);
  void createPmtFilter (NetworkInterface *ni);

public:
  void receiveSection (char *section, int secLen, IFrontendFilter *filter);

  void addPidFilter (unsigned int pid, ITSFilter *filter);
  void addSectionFilter (unsigned int tid, ITSFilter *filter);
  void addStreamTypeFilter (short streamType, ITSFilter *filter);

private:
  void receiveData (char *buff, unsigned int size);

public:
  void processDemuxData ();

private:
  void processDemuxData (char *buff, unsigned int size);

  void updateChannelStatus (short newStatus, Channel *channel);

public:
  static void addPat (Pat *pat);
  static bool isSectionStream (unsigned int pid);
  static void setSectionPid (unsigned int pid); // debug only

private:
  unsigned int hunt (char *buff, unsigned int size);

public:
  short getCaps ();
  bool isReady ();

private:
  void dataArrived ();
  bool waitData ();
};

GINGA_TSPARSER_END

#endif /*DEMUXER_H_*/
