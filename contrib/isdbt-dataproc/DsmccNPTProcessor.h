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

#ifndef NPTPROCESSOR_H_
#define NPTPROCESSOR_H_

#include "system/Thread.h"
using namespace ::ginga::system;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/ITimeBaseProvider.h"
using namespace ::ginga::system;

#include "isdbt-tuner/ISTCProvider.h"
using namespace ::ginga::tuner;

#include "DsmccSectionPayload.h"
#include "DsmccMpegDescriptor.h"
#include "DsmccNPTReference.h"
#include "DsmccTimeBaseClock.h"

GINGA_DATAPROC_BEGIN

class DsmccNPTProcessor : public Thread, public ITimeBaseProvider
{
  struct TimeControl
  {
    double time;
    bool notified;
  };

private:
  static const unsigned short MAX_NPT_VALUE = 47721;
  static const char INVALID_CID = -1;
  static const short NPT_ST_OCCURRING = 0;
  static const short NPT_ST_PAUSED = 1;

  ISTCProvider *stcProvider;
  bool running;
  bool loopControlMin;
  bool loopControlMax;
  unsigned char currentCid;
  unsigned char occurringTimeBaseId;

  pthread_mutex_t loopMutex;
  pthread_mutex_t schedMutex;
  pthread_mutex_t lifeMutex;

  map<unsigned char, DsmccNPTReference *> scheduledNpts;
  map<unsigned char, DsmccTimeBaseClock *> timeBaseClock;
  map<unsigned char, Stc *> timeBaseLife;
  map<unsigned char, set<ITimeBaseProvider *> *> loopListeners;
  map<unsigned char, map<TimeControl *, set<ITimeBaseProvider *> *> *>
      timeListeners;
  set<ITimeBaseProvider *> cidListeners;
  bool reScheduleIt;
  uint64_t firstStc;
  bool isFirstStc;
  bool nptPrinter;
  map<char, DsmccNPTReference *> lastNptList;

public:
  DsmccNPTProcessor (ISTCProvider *stcProvider);
  virtual ~DsmccNPTProcessor ();

  void setNptPrinter (bool nptPrinter);

private:
  uint64_t getSTCValue ();
  void clearUnusedTimebase ();
  void clearTables ();
  void detectLoop ();

public:
  bool addLoopListener (unsigned char cid, ITimeBaseListener *ltn);
  bool removeLoopListener (unsigned char cid, ITimeBaseListener *ltn);

  bool addTimeListener (unsigned char cid, double nptValue,
                        ITimeBaseListener *ltn);

  bool removeTimeListener (unsigned char cid, ITimeBaseListener *ltn);

  bool addIdListener (ITimeBaseListener *ltn);
  bool removeIdListener (ITimeBaseListener *ltn);

  unsigned char getOccurringTimeBaseId ();

private:
  unsigned char getCurrentTimeBaseId ();
  void notifyLoopToTimeListeners ();
  void notifyTimeListeners (unsigned char cid, double nptValue);
  void notifyNaturalEndListeners (unsigned char cid, double nptValue);
  void notifyIdListeners (unsigned char oldCid, unsigned char newCid);
  DsmccTimeBaseClock *getTimeBaseClock (unsigned char cid);
  int updateTimeBase (DsmccTimeBaseClock *clk, DsmccNPTReference *npt);
  DsmccTimeBaseClock *getCurrentTimebase ();
  double getCurrentTimeValue (unsigned char timeBaseId);

public:
  int decodeDescriptors (vector<DsmccMpegDescriptor *> *list);
  double getNPTValue (unsigned char contentId);

private:
  char getNextNptValue (char cid, double *nextNptValue, double *sleepTime);

  bool processNptValues ();
  void run ();
};

GINGA_DATAPROC_END

#endif /* NPTPROCESSOR_H_ */
