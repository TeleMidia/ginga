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
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef ISDBTFRONTEND_H_
#define ISDBTFRONTEND_H_

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "frontend_parameter.h"

#include "IFrontendFilter.h"
#include "Channel.h"

typedef struct lockedFiltersAction
{
  ::br::pucrio::telemidia::ginga::core::tuning::IFrontendFilter *ff;
  bool isAdd;
} ActionsToFilters;

GINGA_TUNER_BEGIN

class ISDBTFrontend : public Thread
{
private:
  static const int IFE_FORCED_MIN_FREQ = 450143000;
  static const int IFE_FORCED_STEP_SIZE = 1000000;

  static const short IFE_MAX_FILTERS = 20;

  static struct pollfd pollFds[ISDBTFrontend::IFE_MAX_FILTERS];

  struct dvb_frontend_parameters params;
  struct dvb_frontend_info info;

  int feFd;
  int dmFd;
  fe_status_t feStatus;
  unsigned int currentFreq;
  static bool firstFilter;
  static vector<IFrontendFilter *> *runningFilters;
  static vector<ActionsToFilters *> *actsToRunningFilters;

public:
  static const string IFE_FE_DEV_NAME;
  static const string IFE_DVR_DEV_NAME;
  static const string IFE_DEMUX_DEV_NAME;

  int dvrFd;
  ISDBTFrontend (int feFd);
  virtual ~ISDBTFrontend ();

  bool hasFrontend ();

private:
  void initIsdbtParameters ();
  void dumpFrontendInfo ();
  void updateIsdbtFrontendParameters ();
  bool isTuned ();

public:
  bool getSTCValue (uint64_t *stc, int *valueType);
  bool changeFrequency (unsigned int frequency);
  void scanFrequencies (vector<Channel *> *channels);

  void attachFilter (IFrontendFilter *filter);
  int createPesFilter (int pid, int pesType, bool compositeFiler);
  void removeFilter (IFrontendFilter *filter);

private:
  void updatePool ();
  void readFilters ();

  void run ();
};

GINGA_TUNER_END

#endif /*ISDBTFRONTEND_H_*/
