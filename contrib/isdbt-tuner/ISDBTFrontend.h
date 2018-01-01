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

#ifndef ISDBTFRONTEND_H_
#define ISDBTFRONTEND_H_

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "IFrontendFilter.h"
#include "Channel.h"

GINGA_BEGIN_DECLS
#include <sys/poll.h>
GINGA_END_DECLS

GINGA_TUNER_BEGIN

struct frontend_parameters
{
  fe_delivery_system_t delivery_system;
  uint32_t frequency;
  fe_spectral_inversion_t inversion;

  union
  {
    struct dvb_qpsk_parameters qpsk;
    struct dvb_qam_parameters qam;
    struct dvb_ofdm_parameters ofdm;
    struct dvb_vsb_parameters vsb;
    struct isdbt_parameters
    {
      uint32_t bandwidth_hz;
      fe_transmit_mode_t transmission_mode;
      fe_guard_interval_t guard_interval;
      uint8_t isdbt_partial_reception;
      uint8_t isdbt_sb_mode;
      uint8_t isdbt_sb_subchannel;
      uint32_t isdbt_sb_segment_idx;
      uint32_t isdbt_sb_segment_count;
      uint8_t isdbt_layer_enabled;
      struct
      {
        uint8_t segment_count;
        fe_code_rate_t fec;
        fe_modulation_t modulation;
        uint8_t interleaving;
      } layer[3];
    } isdbt;
  } u;
};

typedef struct lockedFiltersAction
{
  IFrontendFilter *ff;
  bool isAdd;
} ActionsToFilters;

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
