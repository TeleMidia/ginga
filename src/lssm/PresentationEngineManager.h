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

#ifndef PRESENTATION_ENGINE_MANAGER_H
#define PRESENTATION_ENGINE_MANAGER_H

#include "system/ITimeBaseProvider.h"
using namespace ::ginga::system;

#include "player/INCLPlayer.h"
using namespace ::ginga::player;

#include "system/Thread.h"
using namespace ::ginga::system;

#include "mb/Display.h"
using namespace ::ginga::mb;

#include "player/IPlayerListener.h"
using namespace ::ginga::player;

#include "formatter/PrivateBaseManager.h"
using namespace ::ginga::formatter;

GINGA_LSSM_BEGIN

class PresentationEngineManager
{
private:
  GINGA_MUTEX_DEFN ();

  INCLPlayer *formatter;
  PrivateBaseManager *privateBaseManager;

  static gpointer startPresentationThreadWrapper (gpointer data);

public:
  PresentationEngineManager ();
  ~PresentationEngineManager ();
  void startPresentationThread(void);

private:
  void close ();
  void setTimeBaseInfo (INCLPlayer *nclPlayer);

public:
  bool getIsLocalNcl ();
  void setIsLocalNcl (bool isLocal, void *tuner = NULL);

private:
  INCLPlayer *createNclPlayer (const string &baseId, const string &fname);

public:
  NclPlayerData *createNclPlayerData ();
  void addPlayerListener (const string &nclFile, IPlayerListener *listener);
  void removePlayerListener (const string &nclFile, IPlayerListener *listener);
  bool openNclFile (const string &nclFile);
  bool startPresentation (const string &nclFile, const string &interfId);
  bool pausePresentation (const string &nclFile);
  bool resumePresentation (const string &nclFile);
  bool stopPresentation (const string &nclFile);
  bool setPropertyValue (const string &nclFile,
                         const string &interfaceId,
                         const string &value);
  string getPropertyValue (const string &nclFile, const string &interfaceId);
  bool stopAllPresentations ();
  bool abortPresentation (const string &nclFile);

private:
  void pausePressed ();

public:
  void waitUnlockCondition ();

private:
  void presentationCompleted (const string &formatterId);
  void releaseFormatter (const string &formatterId);

  void updateFormatters (short command);
};

GINGA_LSSM_END

#endif /* PRESENTATION_ENGINE_MANAGER_H */
