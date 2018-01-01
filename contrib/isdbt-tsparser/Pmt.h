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

#ifndef PMT_H_
#define PMT_H_

#include "TransportSection.h"

#include "StreamIdentifierDescriptor.h"

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_TSPARSER_BEGIN

class Pmt : public TransportSection
{
protected:
  map<unsigned int, short> streams;
  map<short, string> streamTypeNames;
  map<unsigned short, unsigned char> componentTags; //<pid,ctag>

  unsigned int pid;
  unsigned int programNumber;
  unsigned int pcrPid;
  bool processed;

public:
  Pmt (unsigned int pid, unsigned int programNumber);
  virtual ~Pmt ();

  bool hasStreamType (short streamType);
  int getPidByStreamType (short streamType);
  void addElementaryStream (unsigned int pid, short esType);
  // void addAITInfo(unsigned int pid, AITInfo* info);
  unsigned int getPid ();
  void setPid (unsigned int pid);
  vector<unsigned int> *copyPidsByTid (unsigned int tid);
  unsigned int getProgramNumber ();
  void setProgramNumber (unsigned int programNumber);
  bool hasPid (unsigned int somePid);
  string getStreamType (unsigned int somePid);
  bool isSectionType (unsigned int pid);
  short getStreamTypeValue (unsigned int somePid);
  string getStreamTypeName (short streamType);
  bool processSectionPayload ();
  bool hasProcessed ();
  map<unsigned int, short> *getStreamsInformation ();
  map<unsigned short, unsigned char> *getComponentTags ();
  unsigned int getPCRPid ();
  unsigned int getDefaultMainVideoPid ();
  unsigned int getDefaultMainAudioPid ();
  unsigned int getDefaultMainCarouselPid ();
  int getTaggedVideoPid (unsigned char idx);
  int getTaggedAudioPid (unsigned char idx);
  void print ();
};

GINGA_TSPARSER_END

#endif /*PMT_H_*/
