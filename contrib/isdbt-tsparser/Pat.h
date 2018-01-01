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

#ifndef PAT_H_
#define PAT_H_

#include "Pmt.h"
#include "TransportSection.h"

struct UnpPmtTime
{
  unsigned int pid;
  double time;
};

GINGA_TSPARSER_BEGIN

class Pat : public TransportSection
{
private:
  static int defaultProgramNumber;
  static int defaultProgramPid;

protected:
  unsigned int currentPid;
  unsigned int currentCarouselId;
  unsigned int carouselComponentTag;
  unsigned int tsPid;

  // Program association table (pmt pid x program number)
  // if i->first == 0 i->second = networkPid
  map<unsigned int, unsigned int> pat;

  set<UnpPmtTime *> unprocessedPmts;

  // PMT Pid TO PMT
  map<unsigned int, Pmt *> programs;

  char *stream;

public:
  Pat ();
  virtual ~Pat ();

  bool hasStreamType (short streamType);
  int getPidByStreamType (short streamType);
  unsigned int getNextPid ();
  unsigned int getNextCarouselId ();
  unsigned int getCarouselComponentTag ();
  bool hasProgramNumber (unsigned int programNumber);
  bool hasPid (unsigned int pid);
  Pmt *getPmtByProgramNumber (unsigned int programNumber);
  unsigned int getTSId ();
  void setTSId (unsigned int newTSId);
  unsigned int getNumOfPrograms ();
  void addProgram (unsigned int pid, unsigned int programNumber);
  void addPmt (Pmt *program);
  void replacePmt (unsigned int pid, Pmt *newPmt);
  unsigned int getFirstProgramNumber ();
  unsigned int getProgramNumberByPid (unsigned int pid);
  short getStreamType (unsigned int pid);
  bool isSectionType (unsigned int pid);

  static void resetPayload (char *payload, int size);

  bool processSectionPayload ();
  bool hasUnprocessedPmt ();
  set<UnpPmtTime *> *getUnprocessedPmtPids ();
  map<unsigned int, Pmt *> *getProgramsInfo ();
  void checkConsistency ();
  unsigned int getDefaultProgramPid ();
  unsigned int getDefaultMainVideoPid ();
  unsigned int getDefaultMainAudioPid ();
  unsigned int getDefaultMainCarouselPid ();

  unsigned short createPatStreamByProgramPid (unsigned short pid,
                                              char **dataStream);

  void print ();
};

GINGA_TSPARSER_END

#endif /*PAT_H_*/
