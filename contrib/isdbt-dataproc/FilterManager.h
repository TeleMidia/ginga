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

#ifndef FILTERMANAGER_H_
#define FILTERMANAGER_H_

#include "isdbt-tsparser/ITransportSection.h"
using namespace ::ginga::tsparser;

#include "SectionFilter.h"
#include "DsmccModule.h"

GINGA_DATAPROC_BEGIN

class FilterManager
{
private:
  map<short, SectionFilter *> streamTypeFilters;
  map<int, SectionFilter *> pidFilters;
  set<string> processedSections;
  map<string, map<unsigned int, ITransportSection *> *> sections;
  map<unsigned int, DsmccModule *> *info;
  bool reading;
  unsigned short blockSize;
  pthread_mutex_t filterMutex;

public:
  FilterManager ();
  virtual ~FilterManager ();
  ITSFilter *createStreamTypeSectionFilter (short streamType,
                                            IDemuxer *demux,
                                            IFilterListener *listener);

  ITSFilter *createPidSectionFilter (int pid, IDemuxer *demux,
                                     IFilterListener *listener);

  void destroyFilter (IDemuxer *demux, short streamType);
  void destroyFilter (IDemuxer *demux, ITSFilter *filter);
  bool processSection (ITransportSection *section);
  void addProcessedSection (const string &sectionName);
  void setInfo (map<unsigned int, DsmccModule *> *info);
  void setBlockSize (unsigned short size);
};

GINGA_DATAPROC_END

#endif /*FILTERMANAGER_H_*/
