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

#ifndef COMMON_CORE_MANAGER_H
#define COMMON_CORE_MANAGER_H

#include "ginga.h"
#include "PresentationEngineManager.h"

GINGA_LSSM_BEGIN

class CommonCoreManager
{
private:
  void *tuner;
  GingaWindowID tuningWindow;
  void *demuxer;
  void *dataProcessor;
  void *nptProvider;
  void *ccUser;
  double ocDelay;
  bool hasOCSupport;
  bool nptPrinter;
  GingaScreenID myScreen;
  PresentationEngineManager *pem;

public:
  CommonCoreManager ();
  ~CommonCoreManager ();
  void addPEM (PresentationEngineManager *pem, GingaScreenID screenId);
  void enableNPTPrinter (bool enableNPTPrinter);
  void setOCDelay (double ocDelay);
  void setInteractivityInfo (bool hasOCSupport);
  void removeOCFilterAfterMount (bool removeIt);
  void setTunerSpec (string tunerSpec);

private:
  void showTunningWindow (GingaScreenID screenId, int x, int y, int w, int h);
  void releaseTunningWindow ();
  IPlayer *createMainAVPlayer (string dstUri, GingaScreenID screenId, int x,
                               int y, int w, int h);

public:
  void tune ();
  void startPresentation ();
};

GINGA_LSSM_END

#endif /* COMMON_CORE_MANAGER_H */
