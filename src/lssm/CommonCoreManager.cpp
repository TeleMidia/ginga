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

#include "ginga.h"
#include "CommonCoreManager.h"

#include "StcWrapper.h"

#if WITH_ISDBT
#include "DataWrapperListener.h"
#include "isdbt-dataproc/DataProcessor.h"
#include "isdbt-dataproc/DataProcessor.h"
#include "isdbt-dataproc/IDsmccObjectListener.h"
#include "isdbt-tsparser/Demuxer.h"
#include "isdbt-tsparser/IDemuxer.h"
#include "isdbt-tsparser/ITSFilter.h"
#include "isdbt-tsparser/PipeFilter.h"
#include "isdbt-tuner/Tuner.h"
using namespace ::ginga::dataproc;
using namespace ::ginga::tsparser;
using namespace ::ginga::tuner;
#endif

#include "mb/DisplayManager.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "player/AVPlayer.h"
#include "player/IPlayer.h"
#include "player/IProgramAV.h"
#include "player/ImagePlayer.h"
#include "player/ProgramAV.h"
using namespace ::ginga::player;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "util/functions.h"
using namespace ::ginga::util;

GINGA_LSSM_BEGIN

CommonCoreManager::CommonCoreManager ()
{
  tuningWindow = 0;
  tuner = NULL;
  demuxer = NULL;
  dataProcessor = NULL;
  ccUser = NULL;
  nptProvider = NULL;
  ocDelay = 0;
  hasOCSupport = true;
  pem = NULL;
  nptPrinter = false;
}

CommonCoreManager::~CommonCoreManager ()
{
  // TODO: release attributes
  clog << "CommonCoreManager::~CommonCoreManager all done" << endl;
}

void
CommonCoreManager::addPEM (PresentationEngineManager *pem)
{
  this->pem = pem;

#if WITH_ISDBT
  tuner = new Tuner ();

  pem->setIsLocalNcl (false, tuner);

  clog << "CommonCoreManager::CommonCoreManager ";
  clog << "creating demuxer" << endl;
  demuxer = new Demuxer ((Tuner *)tuner);

  clog << "CommonCoreManager::CommonCoreManager ";
  clog << "creating data processor" << endl;

  dataProcessor = new DataProcessor ();

  ccUser = pem->getDsmccListener ();

  // Add PEM as a listener of SEs and OCs
  ((DataProcessor *)dataProcessor)
      ->addSEListener (
          "gingaEditingCommands",
          (IDsmccStreamEventListener *)((DataWrapperListener *)ccUser));

  ((DataProcessor *)dataProcessor)
      ->addObjectListener (
          (IDsmccObjectListener *)((DataWrapperListener *)ccUser));

  ((DataProcessor *)dataProcessor)
      ->setServiceDomainListener (
          (IDsmccServiceDomainListener *)((DataWrapperListener *)ccUser));

  ((DataProcessor *)dataProcessor)->setDemuxer ((IDemuxer *)demuxer);

  ((Tuner *)tuner)->setLoopListener ((DataProcessor *)dataProcessor);

#endif // WITH_ISDBT
}

void
CommonCoreManager::enableNPTPrinter (bool enableNPTPrinter)
{
  nptPrinter = enableNPTPrinter;
}

void
CommonCoreManager::setOCDelay (double ocDelay)
{
  this->ocDelay = ocDelay;
}

void
CommonCoreManager::setInteractivityInfo (bool hasOCSupport)
{
  this->hasOCSupport = hasOCSupport;
}

void
CommonCoreManager::removeOCFilterAfterMount (bool removeIt)
{
#if WITH_ISDBT
  if (dataProcessor != NULL)
    {
      ((DataProcessor *)dataProcessor)->removeOCFilterAfterMount (removeIt);
    }
#endif
}

void
CommonCoreManager::setTunerSpec (string tunerSpec)
{
#if WITH_ISDBT
  string ni, ch;
  size_t pos;

  pos = tunerSpec.find_first_of (":");
  if (pos != std::string::npos)
    {
      ni = tunerSpec.substr (0, pos);
      ch = tunerSpec.substr (pos + 1, tunerSpec.length () - pos + 1);
      ((Tuner *)tuner)->setSpec (ni, ch);
    }
#endif
}

void
CommonCoreManager::showTunningWindow (int x, int y,
                                      int w, int h)
{
  SDLSurface* s = 0;
  string tunerImg = "";

  tunerImg = string (GINGA_TUNER_DATADIR) + "tuning.png";
  if (fileExists (tunerImg))
    {
      tuningWindow = Ginga_Display_M->createWindow (x, y, w, h, -10.0);

      s = Ginga_Display_M->createRenderedSurfaceFromImageFile (tunerImg.c_str ());

      int cap = Ginga_Display_M->getWindowCap (tuningWindow, "ALPHACHANNEL");
      Ginga_Display_M->setWindowCaps (tuningWindow, cap);

      Ginga_Display_M->drawWindow (tuningWindow);
      Ginga_Display_M->showWindow (tuningWindow);
      Ginga_Display_M->renderWindowFrom (tuningWindow, s);
      Ginga_Display_M->lowerWindowToBottom (tuningWindow);

      Ginga_Display_M->deleteSurface (s);
      s = 0;
    }
}

void
CommonCoreManager::releaseTunningWindow ()
{
  if (tuningWindow != 0)
    {
      Ginga_Display_M->clearWindowContent (tuningWindow);
      Ginga_Display_M->hideWindow (tuningWindow);
      Ginga_Display_M->deleteWindow (tuningWindow);
      tuningWindow = 0;
    }
}

IPlayer *
CommonCoreManager::createMainAVPlayer (string dstUri,
                                       int x, int y,
                                       int w, int h)
{
  IPlayer *ipav;
  string s;

  ipav = ProgramAV::getInstance ();
  xstrassign (s, "%d,%d,%d,%d", x, y, w, h);
  ipav->setPropertyValue ("setBoundaries", s);
  ipav->setPropertyValue ("createPlayer", "sbtvd-ts://" + dstUri);
  ipav->setPropertyValue ("showPlayer", "sbtvd-ts://" + dstUri);

  return ipav;
}

void
CommonCoreManager::tune ()
{
#if WITH_ISDBT
  clog << "lssm-ccm::cpi tunning..." << endl;
  ((Tuner *)tuner)->tune ();
#endif
}

void
CommonCoreManager::startPresentation ()
{
#if !defined WITH_ISDBT || WITH_ISDBT == 0
  return;
#endif
  IPlayer *ipav = NULL;
  NclPlayerData *data = NULL;
  StcWrapper *sw = NULL;
  NetworkInterface *ni = NULL;
  string dstUri = "dtv_channel.ts";

  data = pem->createNclPlayerData ();

  showTunningWindow (data->x, data->y, data->w, data->h);
  tune ();
  dstUri = ((IDemuxer *)demuxer)->createTSUri (dstUri);

  // Create Main AV
  ipav = createMainAVPlayer (dstUri, data->x, data->y,
                             data->w, data->h);

  delete data;
  clog << "lssm-ccm::sp create av ok" << endl;

  if (dataProcessor != NULL)
    {
      ni = ((Tuner *)tuner)->getCurrentInterface ();
      if (ni != NULL && (ni->getCaps () & DPC_CAN_DECODESTC))
        {
          clog << "lssm-ccm::sp using stc hardware!" << endl;
          ((DataProcessor *)dataProcessor)->setSTCProvider (ni);
        }
      else
        {
          clog << "lssm-ccm::sp using stc wrapper!" << endl;
          sw = new StcWrapper (ipav);
          ((DataProcessor *)dataProcessor)->setSTCProvider (sw);
        }

      nptProvider = ((DataProcessor *)dataProcessor)->getNPTProvider ();
      if (nptProvider != NULL)
        {
          pem->setTimeBaseProvider ((ITimeBaseProvider *)nptProvider);
        }
      else
        {
          clog << "lssm-ccm::sp warning! can't use npt provider" << endl;
        }

      ((DataProcessor *)dataProcessor)->setNptPrinter (nptPrinter);
      if (nptPrinter)
        {
          if (((IDemuxer *)demuxer)
                  ->hasStreamType (STREAM_TYPE_DSMCC_TYPE_C))
            {
              ((IDemuxer *)demuxer)->setNptPrinter (nptPrinter);
              cout << "TS HAS AN NPT STREAM" << endl;
            }
          else
            {
              cout << "NPTPRINTER WARNING!" << endl;
              cout << "TS DOESNT HAVE A STREAM WITH NPT STREAM TYPE"
                   << endl;
            }
          ((IDemuxer *)demuxer)->printPat ();
        }

      // DSM-CC descriptors
      ((DataProcessor *)dataProcessor)
          ->createStreamTypeSectionFilter (STREAM_TYPE_DSMCC_TYPE_D);

      if (hasOCSupport)
        {
          ((DataProcessor *)dataProcessor)
              ->createStreamTypeSectionFilter (STREAM_TYPE_DSMCC_TYPE_B);

          ((DataProcessor *)dataProcessor)
              ->createStreamTypeSectionFilter (STREAM_TYPE_DSMCC_TYPE_C);

          // AIT
          ((DataProcessor *)dataProcessor)
              ->createStreamTypeSectionFilter (STREAM_TYPE_PRIVATE_SECTION);

          clog << "lssm ccm::sp OC support enabled" << endl;
        }
      else if (nptPrinter)
        {
          ((DataProcessor *)dataProcessor)
              ->createStreamTypeSectionFilter (STREAM_TYPE_DSMCC_TYPE_C);
        }
    }

  releaseTunningWindow ();

  ((IDemuxer *)demuxer)->processDemuxData ();

  clog << "lssm ccm::sp all done!" << endl;
}

GINGA_LSSM_END
