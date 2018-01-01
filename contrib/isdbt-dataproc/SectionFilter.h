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

#ifndef SECTIONFILTER_H_
#define SECTIONFILTER_H_

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "isdbt-tsparser/IDemuxer.h"
#include "isdbt-tsparser/ITSPacket.h"
#include "isdbt-tsparser/ITransportSection.h"
#include "isdbt-tsparser/IFilterListener.h"
using namespace ::ginga::tsparser;

GINGA_DATAPROC_BEGIN

struct SectionHandler
{
  // Last section's packet continuityCounter.
  int lastContinuityCounter;
  char sectionHeader[8];
  unsigned int headerSize;
  int recvPack;
  ITransportSection *section;
};

class SectionFilter : public ITSFilter
{
private:
  map<unsigned int, SectionHandler *> sectionPidSelector;

  IFilterListener *listener;
  set<string> processedSections;

  int lastPid;
  map<int, ITransportSection *> hFilteredSections;
  pthread_mutex_t stlMutex;

public:
  SectionFilter ();
  virtual ~SectionFilter ();
  void addPid (unused (int pid)){};

private:
  string
  setDestinationUri (const string &dstUri)
  {
    return dstUri;
  };
  void setSourceUri (unused (const string &srcUri),
                     unused (bool isPipe)){};
  void resetHandler (SectionHandler *handler);
  void ignore (unsigned int pid);
  SectionHandler *getSectionHandler (unsigned int pid);
  void process (ITransportSection *section, unsigned int pid);
  void verifyAndAddData (ITSPacket *pack, bool lastPacket);
  bool verifyAndCreateSection (ITSPacket *pack);
  bool setSectionParameters (ITSPacket *pack);

public:
  void setListener (IFilterListener *listener);
  bool checkProcessedSections (const string &sectionName);
  void addProcessedSection (const string &sectionName);
  void removeProcessedSection (const string &sectionName);
  void clearProcessedSections ();
  bool checkSectionVersion (ITransportSection *section);
  void receiveTSPacket (ITSPacket *pack);
  void receiveSection (char *buf, int len, IFrontendFilter *filter);
  void receivePes (char *buf, int len, IFrontendFilter *filter);
};

GINGA_DATAPROC_END

#endif /*SECTIONFILTER_H_*/
