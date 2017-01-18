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

#include "config.h"
#include "CommonCoreManager.h"

#if WITH_ISDBT
#include "isdbt-tuner/Tuner.h"
#include "isdbt-tsparser/Demuxer.h"
#include "isdbt-tsparser/PipeFilter.h"
#include "isdbt-dataproc/DataProcessor.h"
#endif

#include "util/functions.h"
using namespace ::ginga::util;

#include "system/SystemCompat.h"
using namespace ::ginga::system;

#include "player/ImagePlayer.h"
#include "player/AVPlayer.h"
#include "player/ProgramAV.h"

#if WITH_ISDBT
#include "isdbt-tuner/Tuner.h"
using namespace ::ginga::tuner;

#include "isdbt-tsparser/IDemuxer.h"
#include "isdbt-tsparser/ITSFilter.h"
using namespace ::ginga::tsparser;

#include "isdbt-dataproc/DataProcessor.h"
using namespace ::ginga::dataproc;

#include "isdbt-dataproc/IDsmccObjectListener.h"
using namespace ::ginga::dataproc;

#include "DataWrapperListener.h"
#endif // WITH_ISDBT

#include "mb/LocalScreenManager.h"
#include "mb/ScreenManagerFactory.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "player/IPlayer.h"
#include "player/IProgramAV.h"
using namespace ::ginga::player;

#include "StcWrapper.h"

#include "pthread.h"

GINGA_LSSM_BEGIN

static LocalScreenManager *dm = ScreenManagerFactory::getInstance ();

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
  myScreen = 0;
}

CommonCoreManager::~CommonCoreManager ()
{
  // TODO: release attributes
  clog << "CommonCoreManager::~CommonCoreManager all done" << endl;
}

void
CommonCoreManager::addPEM (PresentationEngineManager *pem,
                           GingaScreenID screenId)
{

  this->myScreen = screenId;
  this->pem = pem;

#if WITH_ISDBT
  tuner = new Tuner (myScreen);

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
CommonCoreManager::showTunningWindow (GingaScreenID screenId, int x, int y,
                                      int w, int h)
{

  GingaSurfaceID s = 0;
  string tunerImg = "";

  tunerImg = string (GINGA_TUNER_DATADIR) + "tuning.png";
  if (fileExists (tunerImg))
    {
      tuningWindow = dm->createWindow (screenId, x, y, w, h, -10.0);

      s = dm->createRenderedSurfaceFromImageFile (screenId,
                                                  tunerImg.c_str ());

      int cap = dm->getWindowCap (myScreen, tuningWindow, "ALPHACHANNEL");
      dm->setWindowCaps (myScreen, tuningWindow, cap);

      dm->drawWindow (myScreen, tuningWindow);
      dm->showWindow (myScreen, tuningWindow);
      dm->renderWindowFrom (myScreen, tuningWindow, s);
      dm->lowerWindowToBottom (myScreen, tuningWindow);

      dm->deleteSurface (s);
      s = 0;
    }
}

void
CommonCoreManager::releaseTunningWindow ()
{
  if (tuningWindow != 0)
    {
      dm->clearWindowContent (myScreen, tuningWindow);
      dm->hideWindow (myScreen, tuningWindow);

      dm->deleteWindow (myScreen, tuningWindow);
      tuningWindow = 0;
    }
}

IPlayer *
CommonCoreManager::createMainAVPlayer (string dstUri,
                                       GingaScreenID screenId, int x, int y,
                                       int w, int h)
{

  IPlayer *ipav;

  clog << "lssm-ccm::cmavp creating player" << endl;

  ipav = ProgramAV::getInstance (screenId);

  // vPid = ((IDemuxer*)demuxer)->getDefaultMainVideoPid();
  // aPid = ((IDemuxer*)demuxer)->getDefaultMainAudioPid();

  // ipav->setPropertyValue("sbtvd-ts://audio", itos(aPid));
  // ipav->setPropertyValue("sbtvd-ts://video", itos(vPid));
  ipav->setPropertyValue ("setBoundaries", itos (x) + "," + itos (y) + ","
                                               + itos (w) + "," + itos (h));

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

  int cpid;

  ITSFilter *mavFilter = NULL;
  IPlayer *ipav = NULL;
  NclPlayerData *data = NULL;
  StcWrapper *sw = NULL;
  NetworkInterface *ni = NULL;
  string dstUri = "dtv_channel.ts";

  data = pem->createNclPlayerData ();

  showTunningWindow (data->screenId, data->x, data->y, data->w, data->h);
  tune ();
  dstUri = ((IDemuxer *)demuxer)->createTSUri (dstUri);

  // Create Main AV
  ipav = createMainAVPlayer (dstUri, data->screenId, data->x, data->y,
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
