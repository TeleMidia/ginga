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

#ifndef IDEMUXER_H_
#define IDEMUXER_H_

#include "isdbt-tuner/Tuner.h"
#include "isdbt-tuner/IFrontendFilter.h"
using namespace ::ginga::tuner;

#include "ITSFilter.h"

GINGA_TSPARSER_BEGIN

// specific packet ids
static G_GNUC_UNUSED const short PAT_PID = 0x0000;
static G_GNUC_UNUSED const short CAT_PID = 0x0001;
static G_GNUC_UNUSED const short TSDT_PID = 0x0001; // TS Description Table
static G_GNUC_UNUSED const short NIT_PID = 0x0010;
static G_GNUC_UNUSED const short SDT_PID = 0x0011;
static G_GNUC_UNUSED const short EIT_PID = 0x0012;
static G_GNUC_UNUSED const short CDT_PID = 0x0029;

// specific section ids
static G_GNUC_UNUSED const short PAT_TID = 0x00;
static G_GNUC_UNUSED const short CAT_TID = 0x01;
static G_GNUC_UNUSED const short PMT_TID = 0x02;
static G_GNUC_UNUSED const short TSDT_TID = 0x03; // TS Description Section
static G_GNUC_UNUSED const short OCI_TID = 0x3B;  // Object Carousel Info
static G_GNUC_UNUSED const short OCD_TID = 0x3C;  // Object Carousel Data
static G_GNUC_UNUSED const short DDE_TID = 0x3D;  // DSM-CC Descriptors
static G_GNUC_UNUSED const short NIT_TID = 0x40;
static G_GNUC_UNUSED const short SDT_TID = 0x42;
static G_GNUC_UNUSED const short EIT_TID = 0x4E;
static G_GNUC_UNUSED const short CDT_TID = 0xC8;
static G_GNUC_UNUSED const short AIT_TID = 0x74;
static G_GNUC_UNUSED const short TOT_TID = 0x73;

// specific stream types
static G_GNUC_UNUSED const short STREAM_TYPE_VIDEO_MPEG1 = 0x01;
static G_GNUC_UNUSED const short STREAM_TYPE_VIDEO_MPEG2 = 0x02;
static G_GNUC_UNUSED const short STREAM_TYPE_AUDIO_MPEG1 = 0x03;
static G_GNUC_UNUSED const short STREAM_TYPE_AUDIO_MPEG2 = 0x04;
static G_GNUC_UNUSED const short STREAM_TYPE_PRIVATE_SECTION = 0x05;
static G_GNUC_UNUSED const short STREAM_TYPE_PRIVATE_DATA = 0x06;
static G_GNUC_UNUSED const short STREAM_TYPE_MHEG = 0x07;
static G_GNUC_UNUSED const short STREAM_TYPE_DSMCC_TYPE_A = 0x0A;
static G_GNUC_UNUSED const short STREAM_TYPE_DSMCC_TYPE_B = 0x0B;
static G_GNUC_UNUSED const short STREAM_TYPE_DSMCC_TYPE_C = 0x0C;
static G_GNUC_UNUSED const short STREAM_TYPE_DSMCC_TYPE_D = 0x0D;
static G_GNUC_UNUSED const short STREAM_TYPE_AUDIO_AAC = 0x0F;
static G_GNUC_UNUSED const short STREAM_TYPE_VIDEO_MPEG4 = 0x10;
static G_GNUC_UNUSED const short STREAM_TYPE_AUDIO_MPEG4 = 0x11;
static G_GNUC_UNUSED const short STREAM_TYPE_VIDEO_H264 = 0x1B;
static G_GNUC_UNUSED const short STREAM_TYPE_AUDIO_AC3 = 0x81;
static G_GNUC_UNUSED const short STREAM_TYPE_AUDIO_DTS = 0x8A;

class IDemuxer : public ITunerListener
{
public:
  virtual ~IDemuxer (){};

  virtual string createTSUri (const string &tsOutputUri) = 0;
  virtual bool hasStreamType (short streamType) = 0;
  virtual void printPat () = 0;
  virtual void setNptPrinter (bool nptPrinter) = 0;

  virtual unsigned int getTSId () = 0;

  static int
  getTableIdFromStreamType (short streamType)
  {
    switch (streamType)
      {
      case STREAM_TYPE_DSMCC_TYPE_B:
        return OCI_TID;

      case STREAM_TYPE_DSMCC_TYPE_C:
        return OCD_TID;

      case STREAM_TYPE_DSMCC_TYPE_D:
        return DDE_TID;

      case STREAM_TYPE_PRIVATE_SECTION:
        return AIT_TID;

      default:
        return -1;
      }
  };

  static int
  getStreamTypeFromTableId (short tid)
  {
    switch (tid)
      {
      case OCI_TID:
        return STREAM_TYPE_DSMCC_TYPE_B;

      case OCD_TID:
        return STREAM_TYPE_DSMCC_TYPE_C;

      case DDE_TID:
        return STREAM_TYPE_DSMCC_TYPE_D;

      case AIT_TID:
        return STREAM_TYPE_PRIVATE_SECTION;

      default:
        return -1;
      }
  };

  virtual void setDestination (short streamType) = 0; // debug purpose only
  virtual void removeFilter (ITSFilter *tsFilter) = 0;
  virtual void addFilter (ITSFilter *tsFilter, int pid, int tid) = 0;
  virtual void addFilter (IFrontendFilter *filter) = 0;
  virtual void receiveSection (char *section, int secLen,
                               IFrontendFilter *filter)
      = 0;

  virtual void addPidFilter (unsigned int pid, ITSFilter *filter) = 0;
  virtual void addSectionFilter (unsigned int tid, ITSFilter *filter) = 0;
  virtual void addStreamTypeFilter (short streamType, ITSFilter *filter)
      = 0;

  virtual int getDefaultMainVideoPid () = 0;
  virtual int getDefaultMainAudioPid () = 0;
  virtual int getDefaultMainCarouselPid () = 0;

  virtual void receiveData (char *buff, unsigned int size) = 0;
  virtual void processDemuxData () = 0;
  virtual void updateChannelStatus (short newStatus, Channel *channel) = 0;

  virtual short getCaps () = 0;

  virtual bool isReady () = 0;
};

GINGA_TSPARSER_END

#endif /*IDEMUXER_H_*/
