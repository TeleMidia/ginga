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

#include "mb/Display.h"
#include "mb/SDLWindow.h"
using namespace ::ginga::mb;

#include "player/IPlayer.h"
#include "player/ImagePlayer.h"
using namespace ::ginga::player;

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
}

void
CommonCoreManager::addPEM (PresentationEngineManager *pem)
{
  this->pem = pem;
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
CommonCoreManager::removeOCFilterAfterMount (arg_unused (bool removeIt))
{
}

void
CommonCoreManager::setTunerSpec (arg_unused (const string &tunerSpec))
{
}

void
CommonCoreManager::showTunningWindow (int x, int y,
                                      int w, int h)
{
  string tunerImg = "";

  tunerImg = string (GINGA_TUNER_DATADIR) + "tuning.png";
  if (fileExists (tunerImg))
    {
      tuningWindow = Ginga_Display->createWindow (x, y, w, h, -10.0);
      tuningWindow->show ();
    }
}

void
CommonCoreManager::releaseTunningWindow ()
{
  if (tuningWindow != 0)
    {
      tuningWindow->hide ();
      delete tuningWindow;
    }
}


void
CommonCoreManager::tune ()
{
}

void
CommonCoreManager::startPresentation ()
{
  return;
}

GINGA_LSSM_END
