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

#include "ginga.h"

#include "ISDBTFrontend.h"
#include "IDataProvider.h"
#include "ISTCProvider.h"
#include "Channel.h"

GINGA_TUNER_BEGIN

const string ISDBTFrontend::IFE_FE_DEV_NAME ("/dev/dvb/adapter0/frontend0");
const string ISDBTFrontend::IFE_DVR_DEV_NAME ("/dev/dvb/adapter0/dvr0");
const string ISDBTFrontend::IFE_DEMUX_DEV_NAME ("/dev/dvb/adapter0/demux0");
struct pollfd ISDBTFrontend::pollFds[ISDBTFrontend::IFE_MAX_FILTERS];

bool ISDBTFrontend::firstFilter = true;
vector<IFrontendFilter *> *ISDBTFrontend::runningFilters
    = (new vector<IFrontendFilter *>);

vector<ActionsToFilters *> *ISDBTFrontend::actsToRunningFilters
    = (new vector<ActionsToFilters *>);

ISDBTFrontend::ISDBTFrontend (int feFd) : Thread ()
{
  int res;
  clog << "ISDBTFrontend::ISDBTFrontend" << endl;

  this->feFd = feFd;
  this->dmFd = -1;
  this->dvrFd = -1;

  res = ioctl (feFd, FE_GET_INFO, &info);
  if (res == -1)
    clog << "ISDBTFrontend::ISDBTFrontend error: " << strerror (errno)
         << endl;

  dumpFrontendInfo ();

  initIsdbtParameters ();
}

ISDBTFrontend::~ISDBTFrontend () {}

bool
ISDBTFrontend::hasFrontend ()
{
  clog << "ISDBTFrontend::hasFrontend" << endl;
  return feFd > 0;
}

void
ISDBTFrontend::initIsdbtParameters ()
{
  clog << "ISDBTFrontend::initIsdbtParameters: Enter" << endl;

  memset (&params, 0, sizeof (dvb_frontend_parameters));

  // for debugging purposes...
  // params.frequency = 533142000; // sbt,  521142000 - rede vida,
  // 599142000 -
  // band;

  params.inversion = (info.caps & FE_CAN_INVERSION_AUTO) ? INVERSION_AUTO
                                                         : INVERSION_OFF;
  params.u.ofdm.code_rate_HP = FEC_AUTO;
  params.u.ofdm.code_rate_LP = FEC_AUTO;
  params.u.ofdm.constellation = QAM_AUTO;
  params.u.ofdm.transmission_mode = TRANSMISSION_MODE_AUTO;
  params.u.ofdm.guard_interval = GUARD_INTERVAL_AUTO;
  params.u.ofdm.hierarchy_information = HIERARCHY_NONE;
  params.u.ofdm.bandwidth = BANDWIDTH_6_MHZ;

  // updateIsdbtFrontendParameters();
}

void
ISDBTFrontend::dumpFrontendInfo ()
{
  clog << "frontend_info:" << endl;
  clog << "name:            " << info.name << endl;
  clog << "fe_type:         " << info.type << endl;
  clog << "freq_min:        " << info.frequency_min << endl;
  clog << "freq_max:        " << info.frequency_max << endl;
  clog << "freq_step:       " << info.frequency_stepsize << endl;
  clog << "symbol_rate_min: " << info.symbol_rate_min << endl;
  clog << "symbol_rate_max: " << info.symbol_rate_max << endl;
  clog << "symbol_rate_tol: " << info.symbol_rate_tolerance << endl;
  clog << "notifier_delay:  " << info.notifier_delay << endl;
  clog << "caps:            " << info.caps << endl;
}

void
ISDBTFrontend::updateIsdbtFrontendParameters ()
{
  clog << "ISDBTFrontend::updateIsdbtFrontendParameters Enter" << endl;

  if (params.frequency == 0)
    return;

  clog << "ISDBTFrontend::updateIsdbtFrontendParameters: frequency: "
       << params.frequency << endl;

  if (params.frequency != 0)
    {
      if (ioctl (feFd, FE_SET_FRONTEND, &params) == -1)
        {
          clog << "ISDBTFrontend::updateIsdbtFrontendParameters: ioctl "
                  "error "
                  "with arg FE_SET_FRONTEND"
               << endl;
        }
    }

  if (dmFd == -1)
    {
      dmFd = open (IFE_DEMUX_DEV_NAME.c_str (), O_RDWR);
      /* WARNING: For now we're just _not_ using the filter infrastructure
       * of
       * the linux kernel, so we just grab the "full" TS */
      if (dmFd < 0)
        {
          clog << "ISDBTFrontend::updateIsdbtFrontendParameters: "
               << IFE_DEMUX_DEV_NAME.c_str ()
               << " could not be opened, bad things will happen!" << endl;
        }

      struct dmx_pes_filter_params filter_dmx;
      filter_dmx.pid = 8192;
      filter_dmx.input = DMX_IN_FRONTEND;
      filter_dmx.output = DMX_OUT_TS_TAP;
      filter_dmx.pes_type = DMX_PES_OTHER;
      filter_dmx.flags = DMX_IMMEDIATE_START;

      if (ioctl (dmFd, DMX_SET_PES_FILTER, &filter_dmx))
        {
          clog << "ISDBTFrontend::updateIsdbtFrontendParameters: ioctl "
                  "error "
                  "with arg IFE_DEMUX_DEV_NAME"
               << endl;
        }
    }

  if (dvrFd == -1)
    {
      // opening DVR device (non-blocking mode), we read TS data in this fd
      dvrFd = open (IFE_DVR_DEV_NAME.c_str (), O_RDONLY | O_NONBLOCK);
      if (dvrFd < 0)
        {
          clog << "ISDBTFrontend::updateIsdbtFrontendParameters: "
               << IFE_DVR_DEV_NAME.c_str ()
               << " could not be opened, bad things will happen!" << endl;
        }
    }
}

bool
ISDBTFrontend::isTuned ()
{
  int value, signal;

  // why do we need this?
  g_usleep (200000);

  clog << "ISDBTFrontend::isTuned" << endl;

  if (ioctl (feFd, FE_READ_STATUS, &feStatus) == -1)
    {
      clog << "ISDBTFrontend::isTuned FE_READ_STATUS failed" << endl;
      return false;
    }

  if (feStatus & FE_HAS_LOCK)
    {
      if (ioctl (feFd, FE_READ_SIGNAL_STRENGTH, &value) == -1)
        {
          clog << "ISDBTFrontend::isTuned FE_READ_SIGNAL_STRENGTH failed"
               << endl;
        }
      else
        {
          signal = value * 100 / 65535;
          clog << "ISDBTFrontend::isTuned: Signal locked, received power "
                  "level is "
               << signal << "%" << endl;
        }
      return true;
    }
  return false;
}

bool
ISDBTFrontend::getSTCValue (uint64_t *stc, int *valueType)
{
  struct dmx_stc *_stc;
  int result, fd;

  if ((fd = open (IFE_DEMUX_DEV_NAME.c_str (), O_RDWR, 644)) < 0)
    {
      perror ("ISDBTFrontend::getSTCValue FD");
      return false;
    }

  _stc = new struct dmx_stc;
  memset (_stc, 0, sizeof (struct dmx_stc));

  result = ioctl (fd, DMX_GET_STC, _stc);
  if (result == -1)
    {
      perror ("ISDBTFrontend::getSTCValue IOCTL");
      delete _stc;
      return false;
    }

  *stc = _stc->stc / _stc->base;
  *valueType = ST_90KHz;

  clog << "ISDBTFrontend::getSTCValue '" << *stc << "'" << endl;
  close (fd);

  delete _stc;
  return true;
}

bool
ISDBTFrontend::changeFrequency (unsigned int frequency)
{
  int i;

  clog << "ISDBTFrontend::changeFrequency 4" << endl;

  currentFreq = frequency;
  params.frequency = currentFreq;
  updateIsdbtFrontendParameters ();

  // try 6 times..
  for (i = 0; i < 6; i++)
    {
      if (isTuned ())
        {
          clog << "ISDBTFrontend::changeFrequency tuned at '";
          clog << currentFreq << "'" << endl;
          return true;
        }
    }

  clog << "ISDBTFrontend::changeFrequency can't change frequency";
  clog << endl;
  return false;
}

void
ISDBTFrontend::scanFrequencies (vector<Channel *> *channels)
{
  int i;
  bool infFm = false, infVhf = false, infUhf = false;
  Channel *channel;

  clog << "ISDBTFrontend::scanFrequencies searching channels" << endl;

  currentFreq = IFE_FORCED_MIN_FREQ;
  info.frequency_stepsize = IFE_FORCED_STEP_SIZE;

  while (currentFreq < info.frequency_max)
    {
      currentFreq = currentFreq + info.frequency_stepsize;

      if (currentFreq < 115000000)
        {
          if (!infFm)
            {
              clog << "Current Frequency at Low VHF band" << endl;
              infFm = true;
            }
          continue;
        }

      if (currentFreq >= 115000000 && currentFreq < 250000000)
        {
          if (!infVhf)
            {
              clog << "Current Frequency at VHF band" << endl;
              infVhf = true;
            }
          continue;
        }

      if (currentFreq >= 250000000 && currentFreq < 863000000)
        {
          if (!infUhf)
            {
              clog << "Current Frequency at UHF band" << endl;
              infUhf = true;
            }
        }
      else
        {
          break;
        }

      params.frequency = currentFreq;
      updateIsdbtFrontendParameters ();

      for (i = 0; i < 4; i++)
        {
          if (isTuned ())
            {
              clog << "ISDBTFrontend::scanFrequencies tuned at '";
              clog << currentFreq << "'" << endl;

              channel = new Channel ();
              channel->setFrequency (currentFreq);

              channels->push_back (channel);
            }
        }
    }

  clog << "ISDBTFrontend::scanFrequencies Finished." << endl;
}

void
ISDBTFrontend::attachFilter (unused (IFrontendFilter *filter))
{
}

int
ISDBTFrontend::createPesFilter (unused (int pid), unused (int pesType), unused (bool compositeFiler))
{
  return -1;
}

void
ISDBTFrontend::removeFilter (unused (IFrontendFilter *filter))
{
}

void
ISDBTFrontend::updatePool ()
{
  int i;
  vector<IFrontendFilter *>::iterator j;

  for (i = 0; i < IFE_MAX_FILTERS; i++)
    {
      pollFds[i].fd = -1;
    }

  clog << "ISDBTFrontend::updatePool: current size is ";
  clog << runningFilters->size () << endl;

  i = 0;
  j = runningFilters->begin ();
  while (j != runningFilters->end ())
    {
      pollFds[i].fd = (*j)->getDescriptor ();
      pollFds[i].events = POLLIN;
      pollFds[i].revents = 0;
      i++;
      ++j;
    }

  clog << "ISDBTFrontend::updatePool: OUT ";
}

void
ISDBTFrontend::readFilters ()
{
  clog << "ISDBTFrontend::readFilters not implemented! ";
}

void
ISDBTFrontend::run ()
{
  clog << "ISDBTFrontend::run no filters running and not yet implemented!"
       << endl;
}

GINGA_TUNER_END
